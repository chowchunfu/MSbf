
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>

#include "typedef.h"

#ifndef ARRAY_CPP
#define ARRAY_CPP


typedef struct {
    int begin[8];
    int* end;

    void clear() {end = begin;}
    void append(int item) {*end++ = item;}
    int size() {return end - begin;}
} iArray8;



struct cArray2d {
    int size;
    int length;
    int width;
    char* data;

    cArray2d() {}

    cArray2d(int _length, int _width) {
        length = _length;
        width = _width;
        size = length * width;

        data = (char*) calloc(size+1, 1);
        if (!data) {printf("array.cpp: fail to alloc memory!"); exit(200);}
    }

    void from_char_p(const char* boardString) {
        std::stringstream ss(boardString);
        readStream(ss);
    }

    void from_file(const std::string filename) {
        std::stringstream ss;
        std::string line;
        std::ifstream f(filename);
        if (!f.is_open()) {
            printf("array.cpp: File not found!");
            exit(0);
        }
        ss << f.rdbuf();
        f.close();
        readStream(ss);
    }

    void readStream(std::stringstream& ss) {
        ss >> width; ss.ignore(1);
        ss >> length;


        size = width * length;
        data = (char*) malloc(size+1);


        char* p = data;

        for (int y = 0; y < length; y++) {
            ss >> p;
            for (int x = 0; x < width; x++) {
                *p = CharToEnum(*p);
                ++p;
            }
        }
        return;
    }


    std::string to_string() { // create a readable string to represent the board
        std::string s;
        s += std::to_string(width) + 'x' + std::to_string(length) + '\n';
        for (int i = 0; i < size; i++) {
            s += EnumToChar(data[i]);
            if ((i+1) % width == 0) s += '\n';
        }

        return s;
    }

    void print() { // print the board string to the console
        if (!data) return;
        std::cout << to_string();
    }

    void printRow(char* row) {
        std::string s;
        for (int i = 0; i < width; i++) s += EnumToChar(row[i]);
        std::cout << s << std::endl;
    }


    void to_file(const std::string& filename) {
        std::ofstream f(filename);
        f << to_string();
        f.close();
    }

    char loc(int x, int y) {
        return data[y * width + x];
    }

};

#endif // ARRAY_CPP
