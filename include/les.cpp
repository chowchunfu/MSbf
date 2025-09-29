#include <unordered_map>

#include "typedef.h"
#include "array.cpp"
#include "le.cpp"


char lesCache[2097152];
char* leSolverWriter;
char* leSolutionWriter;

void initLesCpp() {
    leSolverWriter = lesCache;
    leSolutionWriter = lesCache + 1048576;
}

void printLesMemoryUsage() {
    std::cout << "Les memory usage: " << leSolverWriter - lesCache << " / 1048576" << ", ";
    std::cout  << leSolutionWriter - lesCache - 1048576 << " / 1048576" << std::endl;
}


class LinearEquationSolver : public LinearEquation {
    private:

    iArray8* x2Y;
    iArray8* y2X;
    char* clues;

    public:
    char* solutions;
    int solutionCount;


    public:
    LinearEquationSolver() {}

    LinearEquationSolver(LinearEquation* le) {
        memcpy(this, le, sizeof(LinearEquation));

        x2Y     = (iArray8*)  leSolverWriter;    leSolverWriter += n * sizeof(iArray8);
        y2X     = (iArray8*)  leSolverWriter;    leSolverWriter += m * sizeof(iArray8);
        clues   = (char*)     leSolverWriter;    leSolverWriter += n;

        solutions = leSolutionWriter;
        solutionCount = 0;

        solve();
    }

    void printSolutions() {
        for (int i = 0; i < solutionCount; ++i) printRow(solutions+i*n);
    }

    private:
    void appendSolution(char* sol) {
        ++solutionCount;
        memcpy(leSolutionWriter, sol, n);
        leSolutionWriter += n;
    }

    void constantConstraint() {
        // The content of the hidden cell must not be greater than the constant value.
        for (int i = 0; i < n; ++i) {
            for (int* y = x2Y[i].begin; y < x2Y[i].end; ++y) {
                if (highs[i] > constants[*y]) highs[i] = constants[*y];
            }
        }
    }

    void updateLink() {
        // Link the ones in the matrix horizontally or vertically.
        for (int i = 0; i < n; ++i) x2Y[i].clear();
        for (int i = 0; i < m; ++i) y2X[i].clear();

        // build x2Y / y2X
        for (int y = 0; y < m; ++y) {
            for (int x = 0; x < n; ++x) {
                if (loc(x,y)) {
                    *x2Y[x].end++ = y;
                    *y2X[y].end++ = x;
                }
            }
        }
    }


    bool trivialAnalysis(char* sol) {

        bool repeat = 1;
        while (repeat) {
            repeat = 0;
            for (int y = 0; y < m; ++y) {

                //stat.comparison++;
                int low = 0;
                int high = 0;
                int constant = constants[y];

                for (int* x = y2X[y].begin; x < y2X[y].end; ++x) {
                    if (sol[*x] == HIDDEN) {
                        low += lows[*x];
                        high += highs[*x];
                    } else {
                        low += sol[*x];
                        high += sol[*x];
                    }
                }
                //std::cout << low << high << constant <<  std::endl;

                if (low > constant || high < constant) return false;  // invalid
                if (low == 0 && high == 0) continue;

                if (low == constant) {  // trivial safe
                    for (int *x = y2X[y].begin; x < y2X[y].end; ++x) {
                        if (sol[*x] == HIDDEN) {
                            repeat = 1;
                            sol[*x] = lows[*x];

                        }
                    }


                } else if (high == constant) {  // trivial mine
                    for (int *x = y2X[y].begin; x < y2X[y].end; ++x) {
                        if (sol[*x] == HIDDEN) {
                            repeat = 1;
                            sol[*x] = highs[*x];

                        }
                    }
                }
            }
        }


        return true;    // valid
    }

    int searchHidden(char* sol) {
        int i = 0;
        for (i = 0; i < n; i++) {
            if (sol[i] == 9) return i;
        }
        return -1;
    }


    int DepthFirstSearch() {
        char sols[n*n];
        int X[100];

        memcpy(sols, clues, n);

        int* x = X;
        char* sol = sols;

        *x = searchHidden(sol);
        sol[*x] = lows[*x];

        while (1) {

            memcpy(sol+n, sol, n);
            bool r = trivialAnalysis(sol+n);

            if (r) {
                x[1] = searchHidden(sol+n);

                if (x[1] == -1) {
                    appendSolution(sol+n);
                } else {
                    sol += n; ++x;
                    sol[*x] = lows[*x];

                    continue;
                }
            }

            while (sol[*x] == highs[*x]) {
                sol[*x] = HIDDEN;
                if (sol == sols) return 0;

                sol -= n; --x;
            }

            sol[*x]++;

        }
        return 0;

    }


    IFERROR solve() {
        updateLink();
        constantConstraint();


        memset(clues, HIDDEN, n);
        trivialAnalysis(clues);

        solutionCount = 0;
        if (searchHidden(clues) == -1) {
            appendSolution(clues);
        } else {
            DepthFirstSearch();
        }

        if (!solutionCount) {printf("les.cpp: Solution not found!"); exit(4200);}

        return 0;
    }

};

/*
int main() {
    initLesCpp();
    LinearEquation le;
    le.from_file("testeq.txt");
    LinearEquationSolver les(&le);
    les.printSolutions();
}

*/



