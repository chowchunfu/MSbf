#ifndef UTILS_CPP
#define UTILS_CPP

#include <iostream>
#include <cstring>
#include <vector>
#include <sstream>
#include <unordered_map>

#include "typedef.h"


void SHOW_ERROR(const char* msg, int code) {
    printf(msg);
    exit(code);
}

void print_rect(int* p, int length, int width) {
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < width; j++) {
            printf("%d ", *p);
            p++;
        }
        printf("\n");
    }
}


std::vector<int> convertToVector(const std::string& input) {
    std::vector<int> result;
    std::stringstream ss(input);
    int number;

    while (ss >> number) {
        result.push_back(number);
    }

    return result;
}

void printVector(const std::vector<int>& numbers) {
    for (int num : numbers) {
        std::cout << num << " ";
    }
    std::cout << std::endl;
}


void removeIntegerGaps(int* arr, int n) {
    bool doGapExists[n+1];
    int gaps[n+1];
    int g = 0;

    memset(doGapExists, 1, n);
    for (int i = 0; i < n; i++) doGapExists[arr[i]] = 0;

    for (int i = 0; i < n; i++) {
        g += doGapExists[i];
        gaps[i] = g;
    }
    for (int i = 0; i < n; i++) arr[i] -= gaps[arr[i]];

}


void printcArray(char* begin, char* end) {
    for (char* i = begin; i < end; ++i) std::cout << EnumToChar(*i);
}

void printiArray(int* begin, int* end) {
    std::string s;
    for (int* i = begin; i < end; ++i) s += std::to_string(*i) + ' ';
    std::cout << s;
}


#endif // UTILS_CPP
