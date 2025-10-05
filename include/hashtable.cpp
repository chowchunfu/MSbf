#include <cstdint>
#include <cstring>
#include <iostream>


static size_t HASHTABLE_MEGABYTE = 1024;


typedef struct {
    uint64_t h64;
    int length;
    int value;
} HashDatum; // 16 bytes of data


class HashTable {
    uint64_t stat_collision = 0;
    uint64_t capacity;
    HashDatum** data; // 1 megabyte = 1 door of storage room

    uint64_t hash64(int* begin, int* end, uint64_t h) {
    // Reference: Jenkins's one_at_a_time hash

        for (int* i = begin; i < end; ++i) {
            //h = (h + *i) * 123456791;
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

    public:
    HashTable() {}

    HashTable(size_t megabyte) {

        capacity = megabyte * 65536;
        data = (HashDatum**) malloc(megabyte * sizeof(HashDatum*));

        std::cout << "Allocating memory for hashtable" << std::endl;
        for (int i = 0; i < megabyte; ++i) {
            data[i] = (HashDatum*) calloc(1048576,1);
            if (!data[i]) {printf("hashtable.cpp: Fail to allocate memory!"), exit(1);}
            //memset(data[i], 0, 1048576);
        }
        std::cout << "Allocated " << megabyte << " megabytes with " << capacity << " buckets" << std::endl;
    }


    void insert(int* begin, int* end, int value) {
        uint64_t h = hash64(begin, end, 123456789) % capacity;
        uint64_t door = h / 65536;
        h %= 65536;

        HashDatum* datum = data[door] + h;
        datum->length = end - begin;
        datum->h64 = hash64(begin, end, 987654321);
        datum->value = value;
    }

    int find(int* begin, int* end) {
        uint32_t h = hash64(begin, end, 123456789) % capacity;
        uint64_t door = h / 65536;
        h %= 65536;

        HashDatum* datum = data[door] + h;
        if (datum->length != end - begin) return -1;
        if (datum->h64 != hash64(begin, end, 987654321)) return -1;


        return datum->value;
    }

    int size() {
        int counter = 0;
        for (int i = 0; i < HASHTABLE_MEGABYTE; ++i) {
            for (int j = 0; j < 65536; ++j) {
                counter += data[i][j].h64 != 0;
            }
        }
        return counter;
    }



};


