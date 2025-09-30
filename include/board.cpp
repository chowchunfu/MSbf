#include <assert.h>
#include <string.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>

#ifndef BOARD_CPP
#define BOARD_CPP

#include "typedef.h"
#include "array.cpp"


using namespace std;


typedef struct {
    int index;
    int x;
    int y;
    int neighbours[8];
    int* neighbours_end;  // number of neighbours = (int*) neighbours_end - (int*) neighbours;

} Cell; // a reference class to link between x, y and index;

int isInbounds(int x, int y, int width, int height) { // check if x, y is within the board
    return x >= 0 && x < width && y >= 0 && y < height;
}

Cell* createCells(int width, int height) {
    int size = width * height;
    Cell* cells = (Cell*) malloc(size*sizeof(Cell));
    Cell* cell = cells;

    int Dx[8] = {-1, 0, 1,-1, 1,-1, 0, 1};
    int Dy[8] = {-1,-1,-1, 0, 0, 1, 1, 1};

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // obtain basic information: index / x / y
            int index = y * width + x;
            cell->index = index;
            cell->x = x;
            cell->y = y;

            // assign cell neighbours (stored by index)


            int* end = cell->neighbours;

            for (int n = 0; n < 8; n++) {
                int nx = x + Dx[n];
                int ny = y + Dy[n];
                if (!isInbounds(nx, ny, width, height)) continue;

                *end = ny * width + nx;
                end++;

            }

            cell->neighbours_end = end;
            cell++;
        }
    }

    return cells;
}



struct Board {
    int size;
    int width;
    int height;
    int mineCount;
    char* contents;
    Cell* cells;
    iArray8* allNeighbours;

    Board() {}

    Board(int width, int height, int mineCount) {
        createBoard(width, height, mineCount);
    }


    void createBoard(int _width, int _height, int _mineCount) {
        width = _width;
        height = _height;
        mineCount = _mineCount;

        size = width * height;
        contents = (char*) malloc(size+1);
        memset(contents, 0, size);

        allNeighbours = buildAllNeighbours();




    }

    iArray8* buildAllNeighbours() {

        iArray8* allNeighbours = (iArray8*) malloc(size * sizeof(iArray8));
        int Dx[8] = {-1, 0, 1,-1, 1,-1, 0, 1};
        int Dy[8] = {-1,-1,-1, 0, 0, 1, 1, 1};

        iArray8* neighbours = allNeighbours;

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int i = y * width + x;
                neighbours->clear();

                for (int j = 0; j < 8; ++j) {
                    int nx = x + Dx[j];
                    int ny = y + Dy[j];
                    if (!isInbounds(nx, ny, width, height)) continue;

                    neighbours->append(ny * width + nx);
                }


                ++neighbours;
            }
        }

        return allNeighbours;

    }





    void from_char_p(const char* data) {
        std::stringstream ss(data);
        readStream(ss);
    }

    void from_string(const std::string s) {
        std::stringstream ss(s);
        readStream(ss);
    }

    void from_file(const char* filename) { // import the board from a txt file
        std::stringstream ss;
        std::string line;
        std::ifstream f(filename);
        if (!f.is_open()) {printf("board.cpp: File not found!"); exit(0);}
        ss << f.rdbuf();
        f.close();
        readStream(ss);
    }

    void readStream(std::stringstream& ss) {

        ss >> width; ss.ignore(1);
        ss >> height; ss.ignore(1);
        ss >> mineCount;

        if (width < 1 || width > 100 || height < 1 || height > 100) {printf("board.cpp: invalid board!"); exit(0);}
        createBoard(width, height, mineCount);

        char* p = contents;
        for (int y = 0; y < height; y++) {
            ss >> p;
            for (int x = 0; x < width; x++) p[x] = CharToEnum(p[x]);
            p += width;
        }

        return;
    }


    void fillHidden() {
        memset(contents, 10, size);
    }


    string to_string() { // create a readable string to represent the board
        string s;
        s += std::to_string(width) + 'x' + std::to_string(height) + 'x' + std::to_string(mineCount) + '\n';
        for (int i = 0; i < size; ++i) {
            s += EnumToChar(contents[i]);
            if (i % width == width - 1) s += '\n';
        }

        return s;
    }

    void print() { // print the board string to the console
        std::cout << to_string();
    }

    void printNeighbours() {
        iArray8* neighbours = allNeighbours;

        for (int i = 0; i < 1; ++i) {
            for (int* n = neighbours->begin; n < neighbours->end; ++n)
            ++neighbours;
        }

    }

    void exportMBF(const std::string& path) {
        std::ofstream f(path);
        f << to_string();
        f.close();
    }

};

#endif // BOARD_CPP


