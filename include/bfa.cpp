#include <array>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <vector>
#include <map>
#include <chrono>



#include "utils.cpp"
#include "array.cpp"
#include "hashtable.cpp"
//#include "quicksort.cpp"

const bool IGNORE_DEAD = 0;
const bool PRUNE = 0;

char bfCache[16777216];
char* bfWriter;

//std::map<std::vector<int>, int> mymap;

std::chrono::system_clock::time_point t1;
std::chrono::system_clock::time_point t2;


char* bfAllocate (int size) {
    char* p = bfWriter;
    bfWriter += size;
    if (bfWriter - bfCache > 16777216) {printf("bfa.cpp: Cache overflow!"); exit(4501); }
    return p;
}

struct BruteForceAnalyzerStat {
    double elapsed;
    uint64_t cachebuild;
    uint64_t cachehit;
    uint64_t iteration;
    uint64_t node;

    BruteForceAnalyzerStat() {
        memset(this, 0, sizeof(BruteForceAnalyzerStat));
    }

    void startClock() {
        t1 = std::chrono::system_clock::now();
    }

    void lap() {
        t2 = std::chrono::system_clock::now();
        elapsed += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() / 1e6;
        t1 = t2;
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
    int* Y_sorted;


    int* X;     // indexes
    int* X_end;

    int* x;
    int* x_end;

    int* F;
    int* f;
    int offsets[13];

    char low;
    char high;
    char c;

    int l_min;





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

    void filterProbablySafe(int* _X, int* _X_end, int _x, char* data, int width) {
        int* ff = F;
        X_end = X;
        for (int* i = _X; i < _X_end; ++i) {
            if (*i == _x) continue;
            frequency(ff, *i, data, width);

            if (ff[MINE] != Y_end-Y) {
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

    // FILE FUNCTIONS
    void put(std::ofstream& file) {
        int Y_size = Y_end-Y;
        int X_size = X_end-X;
        int x_offset = x-X;
        int x_size = x_end-x;
        file.write((char*) &Y_size, sizeof(int));
        file.write((char*) Y_sorted, Y_size*sizeof(int));
        file.write((char*) &X_size, sizeof(int));
        file.write((char*) X, X_size * sizeof(int));
        file.write((char*) &x_offset, sizeof(int));
        file.write((char*) &x_size, sizeof(int));
        file.write((char*) F, 12*X_size*sizeof(int));
        file.write((char*) &l_min, sizeof(int));
        file.write((char*) &low, sizeof(char));
        file.write((char*) &high, sizeof(char));
        file.write((char*) &c, sizeof(char));
    }


    void take(std::ifstream& file) {
        int Y_size;
        int X_size;
        int x_offset;
        int x_size;
        file.read((char*) &Y_size, sizeof(int));
        file.read((char*) Y_sorted, Y_size*sizeof(int));
        file.read((char*) &X_size, sizeof(int)); X_end = X + X_size;
        file.read((char*) X, X_size * sizeof(int));
        file.read((char*) &x_offset, sizeof(int)); x = X + x_offset; f = F + x_offset * 12;
        file.read((char*) &x_size, sizeof(int)); x_end = x + x_size;
        file.read((char*) F, 12*X_size*sizeof(int));
        file.read((char*) &l_min, sizeof(int));
        file.read((char*) &low, sizeof(char));
        file.read((char*) &high, sizeof(char));
        file.read((char*) &c, sizeof(char));

        for (int k = 0; k < 12; ++k) offsets[k+1] = offsets[k] + f[k];

        //std::cout << Y_end - Y << ' ';

    }
    // END OF FILE FUNCTIONS

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

    HashTable hashTable;

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

        hashTable = HashTable(HASHTABLE_MEGABYTE);


        if (takeSavestates()) {
            std::cout << "Savestates not found, Starting from scratch" << std::endl;
            firstSavestate();
        } else {
            std::cout << "Savestates found, resuming" << std::endl;

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

        int* begin = (int*) bfAllocate(length * sizeof(int));
        int* end = begin;
        for (int i = 0; i < length; ++i) {*end++ = i;}
        a->create(begin, end);

        a->X_end = a->X;
        for (int i = 0; i < width; ++i) {*a->X_end++ = i;}
        a->filterProbablySafe(a->X, a->X_end, -1, data, width);
        a->x_end = a->X_end;
        a->x = a->X;
        a->f = a->F;

        a->sortY(data, width);
        a->boundary();


    }


    int nextSavestate() {  // core function
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
            winningProbabilities[x] = (double) W[x] / length;
        }
    }




    void solve() {

        std::cout << "Analyzing " << length << " candidates" << std::endl;
        stat.startClock();
        int status;

        while (1) {
            ++stat.iteration;
            status = nextSavestate();
            if (status == -1) break;

            if (stat.iteration % 1000000 == 0) {
                printCompletion();
                if (stat.iteration % 100000000 == 0) {
                    stat.lap();
                    putSavestates();
                }
            }

        }

        stat.lap();
        std::cout << "Brute force analysis complete \n";
        getWins();
        printW();

    }

    // FILE FUNCTIONS
    void putSavestates() {
        std::ofstream f("bfa.savestates", std::ios::binary);
        f.write((char*) &length, sizeof(int));
        f.write((char*) &width, sizeof(int));
        f.write((char*) &stat, sizeof(BruteForceAnalyzerStat));

        int no_of_savestates = a - savestates + 1;
        f.write((char*) &no_of_savestates, sizeof(int));
        for (int i = 0; i < no_of_savestates; ++i) {
            savestates[i].put(f);
        }

        f.close();
    }

    IFERROR takeSavestates() {
        std::ifstream f("bfa.savestates", std::ios::binary);
        if (!f.is_open()) return 1; //printf("bfa.cpp: savestates not found!\n")

        int length_; f.read((char*) &length_, sizeof(int));
        int width_; f.read((char*) &width_, sizeof(int));
        if (length_ != length || width_ != width) return 1;//{printf("bfa.cpp: savestates doesn't match!\n"); return 1;}
        f.read((char*) &stat, sizeof(BruteForceAnalyzerStat));

        int no_of_savestates;
        f.read((char*) &no_of_savestates, sizeof(int));

        a = savestates + no_of_savestates - 1;

        int* begin = (int*) bfAllocate(length * sizeof(int));
        int* end = begin;
        for (int i = 0; i < length; ++i) {*end++ = i;}

        for (int i = 0; i < no_of_savestates; ++i) {
            savestates[i].create(begin, end);
            savestates[i].take(f);
            savestates[i].section(&begin, &end);

        }

        f.close();
        return 0;
    }
    // END OF FILE FUNCTIONS
};
