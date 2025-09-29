#include <array>
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <map>


#include "array.cpp"
#include "hashtable.cpp"
#include "quicksort.cpp"

char bfCache[1048576];
char* bfWriter;

typedef struct {
    int length;
    int l;

} Datum;

void printBfMemoryUseage() {
    std::cout << "bf memory used: " << bfWriter - bfCache << " / 1048576"<< std::endl;
}

char* bfAllocate (int size) {
    char* p = bfWriter;
    bfWriter += size;
    if (bfWriter - bfCache > 1048576) {
        printf("bfa.cpp: Cache overflow!");
        exit(4501);
    }
    return p;
}


struct BruteForceAnalyzerStat {
    int status[5];
    int instance;
    int shallow;
    int iteration;
    int cachebuild;
    int cachehit;
    void reset() {
        memset(this, 0, sizeof(BruteForceAnalyzerStat));
    }
    void print() {
        std::cout << "status: "; for (int i = 0; i < 5; ++i) std::cout << status[i] << ' ';
        std::cout << std::endl;
        std::cout << "instance: " << instance << ", iteration: " << iteration << ", cachebuild: " << cachebuild << ", cachehit: " << cachehit << std::endl;
    }
};



void printR(char* a, int width) {
    char s[width+1];
    s[width] = 0;
    for (int i = 0; i < width; ++i) s[i] = EnumToChar(a[i]);

    std::cout << s << std::endl;
}


struct Savestate{
    int* Y;     // candidates
    int* Y_end;
    int Y_size;

    int* X;     // indexes
    int* X_end;
    int X_size;

    int* x;
    int* x_end;

    int* F;
    int* f;
    int offsets[13];

    int* Y_sorted;

    char c;
    char high;


    int l_min;
    int loss;


    void create(char* data, int length, int width) {
        Y_size = length;

        Y = (int*) bfAllocate(length * sizeof(int));
        Y_sorted = (int*) bfAllocate(length * sizeof(int));

        l_min = Y_size - 1;
        loss = 0;

        Y_end = Y;
        for (int i = 0; i < length; ++i) *Y_end++ = i;

        X_end = X;
        for (int i = 0; i < width; ++i) {
            *X_end++ = i;
        }

        //filterLiving(X, X_end, -1, data, width);

        printF();
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
        X_size = X_end - X;
    }

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

        for (int cc = 0; cc < 12; ++cc) offsets[cc+1] = offsets[cc] + f[cc];

        memcpy(temp, offsets, 12 *sizeof(int));
        for (int* y = Y; y < Y_end; ++y) {
            int cc = data[*y*width+*x];

            Y_sorted[temp[cc]++] = *y;
        }

        c = 0;
    }

    void boundary() {
        for (c = 0; c < 9; ++c) if (f[c]) break;
        for (high = 9; high > 0; --high) if (f[high-1]) break;
    }

    void child(Savestate* a) {
        Y_size = a->f[a->c];

        Y_sorted = (int*) bfAllocate(Y_size * sizeof(int));
        Y = a->Y_sorted + a->offsets[a->c];
        Y_end = a->Y_sorted + a->offsets[a->c+1];

        loss = a->loss + a->f[MINE];
        l_min = Y_size - 1;

        //filterLiving(a->X, a->X_end, *a->x, data, width);

    }

    void sortX() {
        int M[480];
        int x_size = x_end - x;
        for (int i = 0; i < x_size; ++i) M[i] = f[12*i+MINE];
        quicksorter = Quicksorter(M, x_size);
        quicksorter.mimic(x, sizeof(int));
        quicksorter.mimic(f, 12 * sizeof(int));

    }



    std::vector<int> to_vec() {
        std::vector<int> vec;
        for (int* i = Y; i < Y_end; ++i) vec.push_back(*i);
        return vec;
    }

    void printY() {
        for (int* i = Y; i < Y_end; ++i) {
            std::cout << *i << ' ';
        }
        std::cout << std::endl;
    }

    void print(char* data, int width) {
        for (int* i = Y; i < Y_end; ++i) printR(data + *i * width, width);
    }

    void printX() {
        std::cout << "aX: [";
        for (int* i = X; i < X_end; ++i) {
            std::cout << *i << ' ';
        }
        std::cout << "] ";
    }

    void printx() {
        std::cout << "ax: [";

        for (int* i = x; i < x_end; ++i) {
            std::cout << *i << ' ';
        }
        std::cout << "] ";
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



    void printY(char* data, int width) {
        for (int i = 0; i < offsets[9]; ++i) {
            int y = Y_sorted[i];
            printR(data + y*width, width);

        }
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


};




class BruteForceAnalyzer : public cArray2d {
    private:
    BruteForceAnalyzerStat stat;

    char clues[100];

    Savestate* savestates;
    Savestate* a;

    int status;
    void printClues() {
        std::string s;
        for (int i = 0; i < width; ++i) s += EnumToChar(clues[i]);
        std::cout  <<  s << " " << a->Y_size << ' ' << status << "  ";
        std::cout << *savestates->x << "  ";

    }

    void printX() {
        int length = a - savestates + 1; std::cout << "X: [";
        for (int i = 0; i < length; ++i) std::cout << *savestates[i].x << ' ';
        std::cout << "] ";
    }

    void printM() {
        int length = a - savestates + 1; std::cout << "M: [";
        for (int i = 0; i < length; ++i) std::cout << savestates[i].f[11] << " ";
        std::cout << "] ";
    }

    void printL() {
        int length = a - savestates + 1; std::cout << "L: [";
        for (int i = 0; i < length; ++i) std::cout << savestates[i].l_min << " ";
        std::cout << "] ";
    }

    void printLoss() {
        int length = a - savestates + 1; std::cout << "Loss: [";
        for (int i = 0; i < length; ++i) std::cout << savestates[i].loss << " ";
        std::cout << "] ";
    }


    void build() {
        bfWriter = bfCache;
        savestates = (Savestate*) bfAllocate(width * sizeof(Savestate));
        for (int i = 0; i < width; ++i) {
            savestates[i].X = (int*) bfAllocate(width * sizeof(int));
            savestates[i].F = (int*) bfAllocate(12 * width * sizeof(int));
            savestates[i].offsets[0] = 0;

        }
    }

    void firstSavestate() {
        memset(clues, HIDDEN, width);
        status = 0;

        a = savestates;
        a->create(data, length, width);
        a->findSafe();

        a->sortX();
    }


    std::string getRowString(char* row) {
        char s[width+1];
        s[width] = 0;
        memcpy(s, row, width);
        for (int i = 0; i < width; ++i) s[i] = EnumToChar(s[i]);
        return std::string(s);
    }

    void nextSavestate() {
        std::string s;
        std::vector<int> vec;
        int loss;
        int frequency;
        Datum* d;
        int hashValue;
        switch (status) {

            case 0: // analyzing x: assign c
                loss = a->loss + a->f[MINE];
                //if (loss > savestates->l_min) {a->l_min = a->Y_size; status = 4; break;}    // prune 0;

                a->sortY(begin, width);
                a->boundary();
                //a->c = 0;
                clues[*a->x] = a->c;
                status = 1;
                break;

            case 1: // determine frequency;
                status = (a->f[a->c] > 1) ? 2 : 3;
                break;

            case 2: // create children
                if (a->f[MINE] >= a->l_min && a != savestates) {status=3; break;}    // prune 1;

                ++stat.instance;
                a[1].child(a);

                hashValue = hashTable.find(a[1].Y, a[1].Y_end);     // match hashtable
                if (hashValue != -1) {++stat.cachehit; a->f[MINE] += hashValue; status = 3; break;}

                a[1].filterLiving(a->X, a->X_end, *a->x, begin, width);
                if (!a[1].X_size) {a->f[MINE] += a[1].l_min; status = 3; break;}    // all dead


                ++a;
                a->findSafe();
                //if (a->x_end - a->x > 5) a->sortX();

                status = 0;
                break;

            case 3: // increment c
                ++a->c;
                clues[*a->x] = a->c;

                if (a->c == a->high) {
                    clues[*a->x] = HIDDEN;

                    //if (a == savestates) std::cout << "x: " << *a->x  << ", l: " << *a->l << std::endl;

                    status = 4;
                } else {
                    status = 1;
                }

                break;

            case 4: // increment x
                if (a->f[MINE] < a->l_min) a->l_min = a->f[MINE];

                ++a->x;
                a->f += 12;

                if (a->x == a->x_end) {
                    if (a == savestates) {
                        status = -1;
                        break;
                    }

                    ++stat.cachebuild;
                    hashTable.insert(a->Y, a->Y_end, a->l_min);

                    bfWriter = (char*) a->Y_sorted;
                    --a;
                    a->f[MINE] += a[1].l_min;

                    status = 3;
                } else {
                    status = 0;
                }

                break;

        }

        ++stat.iteration;
        ++stat.status[status];
        if (stat.iteration % 1000000 == 0 ) {
            printClues();
            //a->printx();
            //a->printM();
            printL();
            //printM();
            //printLoss();
            //savestates->printM();

            //a->printBoundary();
            std::cout << std::endl;
        }
    }
};


BruteForceAnalyzer bfa;


int main() {
    bfa.from_file("can_12954.txt");
    bfa.stat.reset();

    bfa.build();
    //bfa.print();

    bfa.firstSavestate();

    //for (int i = 0; i < 1000000000; ++i) {
    while (1) {
        bfa.nextSavestate();
        if (bfa.status == -1) break;
    }


    std::cout << "l_min: " << bfa.savestates->l_min << std::endl;
    bfa.savestates->printX(); std::cout << std::endl;
    bfa.savestates->printM(); std::cout << std::endl;

    bfa.stat.print();

    printBfMemoryUseage();

    std::cout << hashTable.stat_collision;
}

