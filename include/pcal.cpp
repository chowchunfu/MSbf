#include <vector>
#include <math.h>
#include "board.cpp"
#include "les.cpp"
#include "ncr.cpp"
#include "utils.cpp"

char pcalCache[2097152];
char* pcalWriter;


const double MAGNITUDES[9] = {0, 0, 0.301029995663981, 0.47712125471967, 0.60205999132796, 0.69897000433602, 0.77815125038364, 0.84509804001426, 0.90308998699194};





void checkPcalMemoryUsage() {
    if (pcalWriter - pcalCache > 2097152) {printf("pcal.cpp: Cache overflow!"); exit(4200); }
}

void printPcalMemoryUsage() {
    std::cout << "Pcal memory usage: " << pcalWriter - pcalCache << " / 2097152" << std::endl;
}


class abc {
    int a;
};


struct CellProperty {
    bool isFlagged;
    bool isHidden;
    bool isOpened;
    bool isHiddenExposed;
    bool isOpenedExposed;
    bool isFloating;
    char reducedNumber;

    iArray8 vision;

    int hiddenIndex;
    int witnessIndex;
    int globalIndex;

    double probability;
};


struct Witness{
    int* begin;
    int* end;
    // before solving linear equation
    iArray8* hiddenVisions;
    iArray8* openedVisions;

    int hiddenIndexCount;
    int openedIndexCount;

    char* highs;
    char* constants;

    LinearEquation le;
    // before solving linear equation
    char* solutions;
    int solutionCount;

    int* weights;
    int* mines;
    int mineMin;
    int mineMax;



    double* probabilities;

    Witness(int* _begin, int* _end, CellProperty* cp) {
        int lh = 0;     // number of hidden cells
        int lo = 0;     // number of opened cells

        for (int* i = _begin; i < _end; ++i) {
            lh += cp[*i].isHiddenExposed;
            lo += cp[*i].isOpenedExposed;
        }

        begin         = (int*)    pcalWriter; pcalWriter += (lh + lo) * sizeof(int);
        end           = (int*)    pcalWriter;
        hiddenVisions = (iArray8*) pcalWriter; pcalWriter += lh * sizeof(iArray8);
        openedVisions = (iArray8*) pcalWriter; pcalWriter += lo * sizeof(iArray8);
        highs         = (char*)   pcalWriter; pcalWriter += lh;
        constants     = (char*)   pcalWriter; pcalWriter += lo;
        probabilities = (double*) pcalWriter; pcalWriter += lh * sizeof(double);



        checkPcalMemoryUsage();
        memcpy(begin, _begin, (lh+lo) * sizeof(int));

        compact(cp);
        buildLinearEquation(cp);
    }

    bool isVisionEquivalent(iArray8* vis1, iArray8* vis2, int l) {
        if (l != vis2->end - vis2->begin) return 0;
        return memcmp(vis1, vis2, l*sizeof(int)) == 0;
    }

    int matchVision(iArray8* vis, iArray8* visions, int* indexCount) {
        int i;
        int l = vis->end - vis->begin;
        for (i = 0; i < *indexCount; i++) {
            if (isVisionEquivalent(vis, visions++, l)) return i;
        }

        memcpy(visions, vis, l * sizeof(int));
        visions->end = visions->begin + l;
        ++*indexCount;

        return i;
    }

    void compact(CellProperty* cp) {
        hiddenIndexCount = 0;
        openedIndexCount = 0;

        for (int* i = begin; i < end; ++i) {

            if (cp[*i].isHiddenExposed) {
                int index = matchVision(&cp[*i].vision, hiddenVisions, &hiddenIndexCount);
                cp[*i].witnessIndex = index;
            } else if (cp[*i].isOpenedExposed) {
                int index = matchVision(&cp[*i].vision, openedVisions, &openedIndexCount);
                cp[*i].witnessIndex = index;
            }
        }
    }




    void buildLinearEquation(CellProperty* cp) {
        le.create(openedIndexCount, hiddenIndexCount);

        // build matrix
        for (int* i = begin; i < end; ++i) {
            if (cp[*i].isOpenedExposed) {
                int y = cp[*i].witnessIndex;
                for (int* n = cp[*i].vision.begin; n < cp[*i].vision.end; ++n) {
                    int x = cp[*n].witnessIndex;
                    le.matrix[y * le.n + x] = 1;
                }
            }
        }

        // build highs and constants
        for (int* i = begin; i < end; ++i) {
            if (cp[*i].isHiddenExposed) highs[cp[*i].witnessIndex]++;  // high
            if (cp[*i].isOpenedExposed) constants[cp[*i].witnessIndex] = cp[*i].reducedNumber; // constant
        }

        memcpy(le.highs, highs, le.n);
        memcpy(le.constants, constants, le.m);

    }

    void solve() {
        // solve linear equation
        LinearEquationSolver les(&le);
        solutions = les.solutions;
        solutionCount = les.solutionCount;

        weights = (int*) pcalWriter; pcalWriter += solutionCount * sizeof(int);
        mines = (int*) pcalWriter; pcalWriter += solutionCount * sizeof(int);

        // get weights and mine used
        char* p = solutions;
        for (int i = 0; i < solutionCount; ++i) {
            int weight = 1;
            int mine = 0;
            for (int x = 0; x < le.n; ++x) {

                weight *= SMALL_nCr[highs[x]][*p];
                mine += *p;
                ++p;
            }
            weights[i] = weight;
            mines[i] = mine;
        }

        // get minimum and maximum mine used
        mineMin = mines[0];
        mineMax = mines[0];

        for (int i = 1; i < solutionCount; ++i) {
            if (mines[i] < mineMin) mineMin = mines[i];
            if (mines[i] > mineMax) mineMax = mines[i];
        }


    }
};




class ProbabilityCalculator : public Board {
private:
    IFERROR error;
    CellProperty* cp;
    Witness* witnesses;
    int witnessCount;

    int mineMin;
    int mineMax;
    double sumProductDensity;

public:
    double globalSolutionCount;
    int floatingCount;
    int remainingMine;
    double density;

public:
    ProbabilityCalculator() {}

    ProbabilityCalculator(Board* board) {
        memcpy(this, board, sizeof(Board));
        pcalWriter = pcalCache;

        cp = (CellProperty*) pcalWriter; pcalWriter += size * sizeof(CellProperty);
        witnesses = (Witness*) pcalWriter; pcalWriter += 50 * sizeof(Witness);

        checkPcalMemoryUsage();

        analyze();

    }

    void printCp(int type) {
        std::string s;

        int c;

        for (int i = 0; i < size; ++i) {
            switch (type) {
            case 0: c = cp[i].isHidden; break;
            case 1: c = cp[i].isFlagged; break;
            case 2: c = cp[i].isOpened; break;
            case 3: c = cp[i].isHiddenExposed; break;
            case 4: c = cp[i].isOpenedExposed; break;
            case 5: c = cp[i].isFloating; break;
            case 6: c = cp[i].vision.size(); break;
            case 7: c = cp[i].reducedNumber; break;
            case 8: c = cp[i].witnessIndex; break;
            case 9: c = cp[i].hiddenIndex; break;
            case 10: c = cp[i].globalIndex; break;
            }
            s += std::to_string(c) + ' ';
            if (i % width == width - 1) s += '\n';

        }

        std::cout << s;
    }

    int getCp(int i, int type) {
        int c;
        switch (type) {
            case 0: c = cp[i].isHidden; break;
            case 1: c = cp[i].isFlagged; break;
            case 2: c = cp[i].isOpened; break;
            case 3: c = cp[i].isHiddenExposed; break;
            case 4: c = cp[i].isOpenedExposed; break;
            case 5: c = cp[i].isFloating; break;
            case 6: c = cp[i].vision.size(); break;
            case 7: c = cp[i].reducedNumber; break;
            case 8: c = cp[i].witnessIndex; break;
            case 9: c = cp[i].hiddenIndex; break;
            case 10: c = cp[i].globalIndex; break;
        }
        return c;
    }


    cArray2d permutateCandidateSolutions() {
        int hiddenIndexCount = assignHiddenIndexes();
        int globalIndexCount = assignGlobalIndexes();

        std::vector<std::vector<int>> N;
        std::vector<int> R;

        for (int i = 0; i < globalIndexCount; ++i) {
            N.push_back({});
            R.push_back(0);
        }
        for (int i = 0; i < size; ++i) {
            if (cp[i].isHidden) N[cp[i].globalIndex].push_back(cp[i].hiddenIndex);
        }

        double dataSize = globalSolutionCount * hiddenIndexCount;

        if (dataSize > 1e8) {std::cout <<"pcal.cpp: too big to generate " << globalSolutionCount << " solutions!"; exit(4300); }

        cArray2d candidates(globalSolutionCount, hiddenIndexCount);

        int y = 0;
        // permutate mines;
        permu(0, 0, 0, N, R, candidates, &y);

        // generate numbers
        for (int i = 0; i < size; ++i) {
            if (!cp[i].isHidden) continue;
            int x = cp[i].hiddenIndex;
            for (int* n = allNeighbours[i].begin; n < allNeighbours[i].end; ++n) {
                if (!cp[*n].isHidden) continue;
                int nx = cp[*n].hiddenIndex;

                for (char* can = candidates.data; can < candidates.data + candidates.size; can += candidates.width) {
                    if (can[x] == MINE) continue;
                    can[x] += can[nx] == MINE;
                }

            }

        }

        return candidates;
    }



private:
    void basicAnalysis() {
        memset(cp, 0, size * sizeof(CellProperty));

        // hidden / flagged / opened

        for (int i = 0; i < size; ++i) {
            cp[i].isHidden = contents[i] == HIDDEN;
            cp[i].isFlagged = contents[i] == FLAG;
            cp[i].isOpened = contents[i] >= 0 && contents[i] < 9;

        }
        // exposed / floating

        for (int i = 0; i < size; ++i) {
            if (cp[i].isHidden) {
                cp[i].isFloating = 1;
                for (int* n = allNeighbours[i].begin; n < allNeighbours[i].end; ++n) {
                    if (cp[*n].isOpened) {
                        cp[*n].isOpenedExposed = 1;
                        cp[i].isHiddenExposed = 1;
                        cp[i].isFloating = 0;
                    }
                }
            }
        }
        // vision

        for (int i = 0; i < size; ++i) {
            cp[i].vision.clear();
            if (cp[i].isHiddenExposed) {
                for (int* n = allNeighbours[i].begin; n < allNeighbours[i].end; ++n) {
                    if (cp[*n].isOpenedExposed) cp[i].vision.append(*n);
                }
            } else if (cp[i].isOpenedExposed) {
                for (int* n = allNeighbours[i].begin; n < allNeighbours[i].end; ++n) {
                    if (cp[*n].isHiddenExposed) cp[i].vision.append(*n);
                }
            }
        }

        // reducedNumber
        for (int i = 0; i < size; ++i) {
            if (cp[i].isOpenedExposed) {
                int num = contents[i];
                for (int* n = allNeighbours[i].begin; n < allNeighbours[i].end; ++n) num -= cp[*n].isFlagged;
                cp[i].reducedNumber = num;
            }
        }

        // number of hidden / floating / remaining mines
        floatingCount = 0;
        remainingMine = mineCount;
        for (int i = 0; i < size; ++i) {
            floatingCount += cp[i].isFloating;
            remainingMine -= cp[i].isFlagged;
        }


    }

    void chainWitnessMembers(int i, bool* unused, int** end) {
        for (int* n = cp[i].vision.begin; n < cp[i].vision.end; n++) {
            if (unused[*n]) {
                unused[*n] = 0;
                **end = *n;
                ++*end;
                chainWitnessMembers(*n, unused, end);
            }
        }
    }


    void determineWitnesses() {
        witnessCount = 0;
        bool unused[size];
        int begin[size];
        int* end;
        memset(unused, 1, size);

        for (int i = 0; i < size; i++) {
            if (unused[i] && cp[i].isHiddenExposed) {
                end = begin;
                chainWitnessMembers(i, unused, &end);

                witnesses[witnessCount] = Witness(begin, end, cp);
                ++witnessCount;
            }
        }
    }

    void solveWitnesses() {
        initLesCpp();
        for (int i = 0; i < witnessCount; ++i) witnesses[i].solve();
    }


    void mineBoundary() {
        mineMin = 0;
        mineMax = 0;
        for (int i = 0; i < witnessCount; ++i) {
            mineMin += witnesses[i].mineMin;
            mineMax += witnesses[i].mineMax;
        }
    }

    void accum(int i, int weight, int mine, double* weight2s) {

        if (i == witnessCount) {
            int floatingMine = remainingMine - mine;
            if (floatingMine > floatingCount) return;

            double weight2 = weight2s[mine - mineMin];
            globalSolutionCount += weight * weight2;
            sumProductDensity += weight * weight2 * floatingMine;
            return;
        }
        Witness* wit = witnesses + i;
        for (int sId = 0; sId < wit->solutionCount; ++sId) {
            accum(i+1, weight * wit->weights[sId], mine + wit->mines[sId], weight2s);
        }

    }

    void calculateGlobalSolutionCount() {
        int mineRange;
        mineRange = mineMax - mineMin + 1;

        // compute weight2 reference
        double weight2s[mineRange];

        for (int m = 0; m < mineRange; ++m) {
            int floatingMine = remainingMine - mineMin - m;
            weight2s[m] = C(floatingCount, floatingMine);

        }

        globalSolutionCount = 0;
        sumProductDensity = 0;
        density = 0;
        accum(0, 1, 0, weight2s);

        if (floatingCount) density = (double) sumProductDensity / globalSolutionCount / floatingCount;
        //for (int i = 0; i < mineRange; ++i) std::cout << weight2s[i] << ' ';

    }


    int assignHiddenIndexes() {
        int idx = 0;
        for (int i = 0; i < size; ++i) {
            if (cp[i].isHidden) cp[i].hiddenIndex = idx++;
        }
        return idx;
    }


    int assignGlobalIndexes() {
        int idx = 0;
        Witness* wit = witnesses;
        for (int witId = 0; witId < witnessCount; ++witId) {
            for (int* i = wit->begin; i < wit->end; ++i) {
                if (cp[*i].isHidden) cp[*i].globalIndex = idx + cp[*i].witnessIndex;
            }
            idx += wit->hiddenIndexCount;
            ++wit;
        }
        if (floatingCount) {
            for (int i = 0; i < size; ++i) {
                if (cp[i].isFloating) cp[i].globalIndex = idx;
            }
            ++idx;
        }
        return idx;
    }

    void permu(int i, int idx, int mine, std::vector<std::vector<int>>& N, std::vector<int>& R, cArray2d candidates, int* y) {

        if (i == witnessCount) {
            int floatingMine = remainingMine - mine;

            if (floatingMine < 0 || floatingMine > floatingCount) return;

            R[idx] = floatingMine;
            std::vector<std::vector<int>> combinations = getCombinations(N, R);


            for (std::vector<int> comb : combinations) {
                for (int num : comb) {
                    candidates.data[*y*candidates.width+num] = MINE;
                }
                ++*y;
            }
            return;
        }
        Witness* wit = witnesses + i;
        for (int sId = 0; sId < wit->solutionCount; ++sId) {
            for (int x = 0; x < wit->le.n; ++x) {
                R[idx+x] = wit->solutions[wit->le.n*sId+x];
            }
            permu(i+1, idx+wit->le.n, mine + wit->mines[sId], N, R, candidates, y);
        }

    }



    void analyze() {
        basicAnalysis();

        determineWitnesses();
        solveWitnesses();
        mineBoundary();
        calculateGlobalSolutionCount();

    }




};





