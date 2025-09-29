#ifndef TYPEDEF_H
#define TYPEDEF_H


enum {
    NUM0 = 0,
    NUM1 = 1,
    NUM2 = 2,
    NUM3 = 3,
    NUM4 = 4,
    NUM5 = 5,
    NUM6 = 6,
    NUM7 = 7,
    NUM8 = 8,
    HIDDEN = 9,
    FLAG = 10,
    MINE = 11,
};

typedef bool IFERROR;
typedef bool IFEND;





char CharToEnum(char c) {
    if (c == 'F') return FLAG;
    if (c == 'H') return HIDDEN;
    if (c == 'M') return MINE;
    return c - '0';
}

char EnumToChar(char c) {
    if (c == FLAG) return 'F';
    if (c == HIDDEN) return 'H';
    if (c == MINE) return 'M';
    return c + '0';
}

#endif // TYPEDEF_H
