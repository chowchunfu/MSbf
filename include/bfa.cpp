#include <array>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <vector>
#include <map>
#include <math.h>

#include "utils.cpp"
#include "array.cpp"
#include "hashtable.cpp"
#include "quicksort.cpp"

const bool IGNORE_DEAD = 0;
const bool PRUNE = 0;

char bfCache[16777216];
char* bfWriter;

std::map<std::vector<int>, int> mymap;


char* bfAllocate (int size) {
    char* p = bfWriter;
    bfWriter += size;
    if (bfWriter - bfCache > 16777216) {printf("bfa.cpp: Cache overflow!"); exit(4501); }
    return p;
}

struct BruteForceAnalyzerStat {
    uint64_t cachebuild;
    uint64_t cachehit;
    uint64_t iteration;
    uint64_t node;

    BruteForceAnalyzerStat() {
        memset(this, 0, sizeof(BruteForceAnalyzerStat));
    }

    void print() {
        std::string s =
            "cachebuid: " + std::to_string(cachebuild) + ", " +
            "cachehit: " + std::to_string(cachehit) + ", " +
            "iteration: " + std::to_string(iteration) + ", " +
            "node: " + std::to_string(node) + ", ";
        std::cout << s << std::endl;
    }
};



struct Savestate{
    bool pruneMode = 0;

    int* Y;     // candidates
    int* Y_end;

    int* X;     // indexes
    int* X_end;

    int* x;
    int* x_end;

    int* F;
    int* f;
    int offsets[13];

    int* Y_sorted;

    char c;
    char low;
    char high;


    int l_min;

    void first(char* data, int length, int width) {


        Y = (int*) bfAllocate(length * sizeof(int));
        Y_sorted = (int*) bfAllocate(length * sizeof(int));

        Y_end = Y; for (int i = 0; i < length; ++i) *Y_end++ = i;
        X_end = X; for (int i = 0; i < width; ++i) *X_end++ = i;

        l_min = length - 1;


        if (IGNORE_DEAD) {
            filterLiving(X, X_end, -1, data, width);
        } else {
            for (int i = 0; i < width; ++i) frequency(F+12*i, i, data, width);
        }
        x_end = X_end;
        x = X;
        f = F;
    }

    void frequency(int* f, int x, char* data, int width) {
        memset(f, 0, 12*sizeof(int));
        for (int* y = Y; y < Y_end; ++y) ++f[data[*y*width+x]];
    }

    void filterLiving(int* _X, int* _X_end, int _x, char* data, int width) {
        int unique;
        int* ff = F;

        X_end = X;
        for (int* i = _X; i < _X_end; ++i) {
            if (*i == _x) continue;
            frequency(ff, *i, data, width);
            unique = 0;
            for (int c = 0; c < 9; ++c) unique += ff[c] != 0;
            if (unique > 1) {
                *X_end++ = *i;
                ff += 12;
            }
        }
    }

    bool isAllDead() {return X_end == X;}


    void findSafe() {
        f = F;
        for (x = X; x < X_end; ++x) {
            if (!f[MINE]) {
                x_end = x+1;
                return;
            }
            f += 12;
        }


        x_end = X_end;
        x = X;
        f = F;
    }

    void sortY(char* data, int width) {
        int temp[12];
        int k;
        for (k = 0; k < 12; ++k) offsets[k+1] = offsets[k] + f[k];

        memcpy(temp, offsets, 12 *sizeof(int));

        for (int* y = Y; y < Y_end; ++y) {
            int k = data[*y*width+*x];

            Y_sorted[temp[k]++] = *y;
        }
    }

    void boundary() {
        // determine the maximum and minimum value of c
        for (low = 0; low < 9; ++low) if (f[low]) break;
        for (high = 9; high > 0; --high) if (f[high]) break;
        c = low;
    }

    IFEND next(char* data, int width) {
        ++c;
        if (c == high+1) {
            if (f[MINE] < l_min) l_min = f[MINE];

            ++x;
            f += 12;
            if (x == x_end) {bfWriter = (char*) Y_sorted; return 1;}

            sortY(data, width);
            boundary();

        }




        return 0;
    }

    void section(int** begin, int** end) {
        *begin = Y_sorted + offsets[c];
        *end = Y_sorted + offsets[c+1];
    }

    void create(int* begin, int* end) {
        int length = end - begin;
        Y_sorted = (int*) bfAllocate(length * sizeof(int));
        Y = begin;
        Y_end = end;

        l_min = length - 1;
    }
    /*
    void from_parent(Savestate* a, char* data, int width) {
        a->section(&Y, &Y_end);
        int length = Y_end - Y;
        Y_sorted = (int*) bfAllocate(length * sizeof(int));
        filterLiving(a->X, a->X_end, *a->x, data, width);
    }

    void sortX() {
        int M[480];
        int x_size = x_end - x;
        for (int i = 0; i < x_size; ++i) M[i] = f[12*i+MINE];
        quicksorter = Quicksorter(M, x_size);
        quicksorter.mimic(x, sizeof(int));
        quicksorter.mimic(f, 12 * sizeof(int));

    }
    */

    std::vector<int> to_vec() {
        std::vector<int> vec;
        for (int* i = Y; i < Y_end; ++i) vec.push_back(*i);
        return vec;
    }

    // PRINT FUNCTIONS
    void print(char* data, int width) {
        for (int* i = Y; i < Y_end; ++i) {
            printcArray(data+*i*width, data+(*i+1)*width);
            std::cout << std::endl;
        }
    }


    void printX() { std::cout << "aX: ["; printiArray(X, X_end); std::cout << "]";}
    void printx() { std::cout << "ax: ["; printiArray(x, x_end); std::cout << "]";}
    void printY() { std::cout << "aY: ["; printiArray(Y, Y_end); std::cout << "]";}

    double completion(double* portion) {
        double completed = 0;
        completed += (double) *portion * (x-X)/(X_end-X);
        *portion /= X_end-X;
        completed += (double) *portion * (c-low)/(high-low+1);
        *portion /= (high-low+1);
        return completed;
    }



    void printM() {
        std::cout << "aM: [";
        int* k = F;
        for (int* i = X; i < X_end; ++i) {
            std::cout << k[11] << ' ';
            k += 12;
        }
        std::cout << "] ";
    }
    void printm() {
        std::cout << "am: [";
        int* k = f;
        for (int* i = x; i < x_end; ++i) {
            std::cout << k[11] << ' ';
            k += 12;
        }
        std::cout << "] ";
    }

    void printF() {

        std::cout << "aF: [";
        int* k = f;
        for (int* i = X; i < X_end; ++i) {
            for (int j = 0; j < 12; ++j) {
                std::cout << k[j] << ' ';
            }
            k += 12;
            std::cout << std::endl;
        }
        std::cout << "] ";
    }
    // END OF PRINT FUNCTIONS
};




class BruteForceAnalyzer : public cArray2d{
private:
    char* clues;
    Savestate* savestates;
    Savestate* a;
    int status;

public:
    BruteForceAnalyzerStat stat;
    int* W;
    double* winningProbabilities;

public:
    BruteForceAnalyzer() {}
    BruteForceAnalyzer(cArray2d* candidates) {
        memcpy(this, candidates, sizeof(cArray2d));
        bfWriter = bfCache;

        clues = (char*) bfAllocate(width);
        W = (int*) bfAllocate(width * sizeof(int));
        winningProbabilities = (double*) bfAllocate(width * sizeof(double));

        savestates = (Savestate*) bfAllocate(width * sizeof(Savestate));
        for (int i = 0; i < width; ++i) {
            savestates[i].pruneMode = (i == 0) ? 0 : 1;
            savestates[i].X = (int*) bfAllocate(width * sizeof(int));
            savestates[i].F = (int*) bfAllocate(12 * width * sizeof(int));
            savestates[i].offsets[0] = 0;

        }

        solve();
    }

    // PRINT FUNCTIONS
    void printM() {std::cout << "M: ["; for (Savestate* i = savestates; i <= a; ++i) std::cout << i->f[MINE] << ' '; std::cout << "] ";}
    void printLMin() {std::cout << "L: ["; for (Savestate* i = savestates; i <= a; ++i) std::cout << i->l_min << ' '; std::cout << "] ";}
    void printW() {std::cout << "W: ["; printiArray(W, W+width); std::cout << "]\n"; }


    void printClues() {
        memset(clues, HIDDEN, width);
        for (Savestate* i = savestates; i <= a; ++i) {
            clues[*i->x] = i->c;
        }
        std::cout << "["; printcArray(clues, clues+width); std::cout <<  "] ";
    }

    void printCompletion() {
        double completion = 0;
        double portion = 1;

        printClues();

        for (Savestate* i = savestates; i <= a; ++i) {
            completion += i->completion(&portion);
        }


        std::cout << std::fixed << std::setprecision(2) << completion * 100 << "%";
        std::cout << std::endl;
    }
    // END OF PRINT FUNCTIONS

private:

    void firstSavestate() {
        ++stat.node;
        a = savestates;
        a->first(data, length, width);
        a->sortY(data, width);
        a->boundary();

        //a->filterLiving(a->X, a->X_end, -1, data, width);
        //a->sortX();
    }

    int nextSavestate() {
        int r;
        int hashValue;
        int* begin;
        int* end;
        std::vector<int> vec;

        a->section(&begin, &end);
        if (end - begin < 2) goto increment;
        if (a->f[MINE] >= a->l_min && (a != savestates || PRUNE)) goto increment;

        // match dictionary
        ++stat.node;
        hashValue = hashTable.find(begin, end);
        if (hashValue != -1) {++stat.cachehit; a->f[MINE] += hashValue; goto increment;}

        // create children
        a[1].create(begin, end);
        a[1].filterLiving(a->X, a->X_end, *a->x, data, width);

        // If all tiles are dead
        if (a[1].isAllDead()) { a->f[MINE] += a[1].l_min; goto increment;}

        // Further analysis required: Assign x, c

        ++a;
        a->findSafe();
        a->sortY(data, width);
        a->boundary();

        return 0;

        increment: // (No further analysis required)

        while (a->next(data, width)) {
            if (a == savestates) return -1;

            ++stat.cachebuild;
            hashTable.insert(a->Y, a->Y_end, a->l_min);

            --a;
            a->f[MINE] += a[1].l_min;
        }

        return 0;
    }


    void getWins() {
        int X_size = savestates->X_end - savestates->X;
        memset(W, 0, width * sizeof(int));
        for (int i = 0; i < X_size; ++i) {
            int x = savestates->X[i];
            W[x] = length - savestates->F[i*12+MINE];
            winningProbabilities[i] = (double) W[i] / length;
        }
    }

    void solve() {
        std::cout << "Analyzing " << length << " candidates" << std::endl;
        firstSavestate();

        //savestates->x_end = savestates->x+1;
        int status;
        double completed;

        while (1) {
            ++stat.iteration;
            status = nextSavestate();
            if (status == -1) break;
            if (stat.iteration % 1000000 == 0) printCompletion();

        }

        std::cout << "Brute force analysis complete \n";
        getWins();
        printW();

    }

};
