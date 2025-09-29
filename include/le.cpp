#include <iostream>
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>


struct LinearEquation {
    int size;
    int m;
    int n;
    char* lows;
    char* highs;
    char* constants;
    char* matrix;

    LinearEquation() {}

    LinearEquation(int _m, int _n) {
        create(_m, _n);
    }

    void create(int _m, int _n) {

        m = _m;
        n = _n;
        size = m * n;
        lows = (char*) calloc(n, 1);
        highs = (char*) calloc(n, 1);
        constants = (char*) calloc(m, 1);
        matrix = (char*) calloc(size+1, 1);
    };

    void from_string(const char* eqString) {
        std::stringstream ss(eqString);
        readStream(ss);
    }

    void from_file(const char* filename) {
        std::stringstream ss;
        std::ifstream f(filename);
        if (!f.is_open()) { printf("le.cpp: File not found!"); exit(0);}
        ss << f.rdbuf();
        f.close();
        readStream(ss);
    }

    void readStream(std::stringstream& ss) {

        ss >> m; ss.ignore(1);
        ss >> n;
        create(m,n);

        size = m * n;

        ss >> lows;      for (int i = 0; i < n; i++) lows[i] -= '0';
        ss >> highs;     for (int i = 0; i < n; i++) highs[i] -= '0';
        ss >> constants; for (int i = 0; i < m; i++) constants[i] -= '0';

        char* p = matrix;
        for (int i = 0; i < m; i++) {
            ss >> p;
            p += n;
        }
        for (int i = 0; i < size; i++) matrix[i] -= '0';


    }

    void from_data(int _m, int _n, char* _matrix, char* _constants, char* _lows, char* _highs) {
        create(_m, _n);

        memcpy(matrix, _matrix, size);
        memcpy(constants, _constants, m);
        memcpy(lows, _lows, n);
        memcpy(highs, _highs, n);

    }



    std::string to_string() {
        std::string s;
        s += std::to_string(m) + 'x' + std::to_string(n) + '\n';
        for (int i = 0; i < n; i++) s += lows[i] + '0'      ; s += '\n';
        for (int i = 0; i < n; i++) s += highs[i] + '0'     ; s += '\n';
        for (int i = 0; i < m; i++) s += constants[i] + '0' ; s += '\n';

        char* p = matrix;
        for (int row = 0; row < m; row++) {
            for (int col = 0; col < n; col++) {
                s += (*p) + '0';
                p++;
            }
            s += '\n';
        }
        return s;

    }

    void print() { std::cout << to_string(); }

    void to_file(const char* filename) {
        std::ofstream f(filename);
        f << to_string();
        f.close();
    }

    char* getRow(int y) { return matrix + y * n; }


    void printRow(char* row) {
        std::string s;
        for (int x = 0; x < n; x++) {
            s += EnumToChar(row[x]);
        }
        std::cout << s << std::endl;
    }


    void printEquation(int y) {
        for (int x = 0; x < n; x++) {
            if (loc(x,y)) {
                std::cout << "x_" << x << ' ';
                //if (x > 0) std::cout << "+ ";
            }
        }
        std::cout << "= " << (int) constants[y] << std::endl;

    }

    char loc(int x, int y) {
        return matrix[y * n + x];
    }

    void edit(int x, int y) {
        matrix[y * n + x] = 1;
    }


    double complexity() {
        double complexity = 1;
        for (int i = 0; i < n; i++) {
            complexity *= highs[i] - lows[i] + 1;
        }
        return complexity;
    }

    int complexityScore() {
        int score = 0;
        for (int i = 0; i < n; i++) {
            int space = highs[i] - lows[i] + 1;
            //score += COMPLEXITIY_SCORES[space];
        }
        return score;
    }




};









