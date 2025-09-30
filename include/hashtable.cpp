#include <cstdint>
#include <cstring>
#include <iostream>




uint32_t hash24(int* begin, int* end) {
    // Reference: Jenkins's one_at_a_time hash
    uint32_t h = 123456789;
    for (int* i = begin; i < end; ++i) {
        h += *i;
        h += h << 10;
        h ^= h >> 6;
    }
    h <<= 8;
    h >>= 8;
    return h;
}

uint32_t hash26(int* begin, int* end) {
    // Reference: Jenkins's one_at_a_time hash
    uint32_t h = 123456789;
    for (int* i = begin; i < end; ++i) {
        h += *i;
        h += h << 10;
        h ^= h >> 6;
    }
    h <<= 6;
    h >>= 6;
    return h;
}

uint64_t hash64(int* begin, int* end) {
    // Reference: Jenkins's one_at_a_time hash
    uint64_t h = 987654321;
    for (int* i = begin; i < end; ++i) {
        h += *i;
        h += h << 10;
        h ^= h >> 6;
        /*
        h = (~h) + (h << 21); // key = (key << 21) - key - 1;
        h = h ^ (h >> 24);
        h = (h + (h << 3)) + (h << 8); // key * 265
        h ^= h >> 14;
        h = (h + (h << 2)) + (h << 4); // key * 21
        h = h ^ (h >> 28);
        h = h + (h << 31);
        */
    }



    return h;
}


typedef struct {
    uint64_t h64;
    int value;
} HashDatum;


struct HashTable {
    uint64_t stat_collision = 0;
    HashDatum data[67108864];

    void clear() {
        memset(data, 0, 16800000*sizeof(HashDatum));
    }

    void insert(int* begin, int* end, int value) {
        uint32_t h = hash26(begin, end);
        uint64_t h64 = hash64(begin, end);

        data[h].h64 = h64;
        data[h].value = value;
    }

    int find(int* begin, int* end) {
        uint32_t h = hash26(begin, end);
        uint64_t h64 = hash64(begin, end);

        if (data[h].h64 == h64) {
            return data[h].value;
        } else if (data[h].h64) {
            ++stat_collision;
        }

        return -1;
    }

    int size() {
        int counter = 0;
        for (int i = 0; i < 67108864; ++i) counter += data[i].h64 != 0;
        return counter;
    }



};

HashTable hashTable;

