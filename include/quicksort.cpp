#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void generateRandomArray(int arr[], int size) {
    for (int i = 0; i < size; i++) {
        arr[i] = (rand() % 100) + 1; // Random number between 1 and 100
    }
}

int counter = 0;


class Quicksorter {
    private:
    int indices[480];
    int size;

    public:
    Quicksorter() {}

    Quicksorter(int* values, int _size) {
        size = _size;
        for (int i = 0; i < size; ++i) indices[i] = i;;
        quicksort(values, 0, size-1);
    }

    void mimic(void* values, int byteSize) {
        char* p = (char*) values;
        char temp[23040];

        for (int i = 0; i < size; ++i) {
            int index = indices[i];
            memcpy(temp + i*byteSize, p + index*byteSize, byteSize);
        }
        memcpy(p, temp, size * byteSize);

    }



    private:
    void swap(int *a, int *b) {
        int temp = *a;
        *a = *b;
        *b = temp;
    }

    int partition(int* values, int low, int high) {
        int pivot = values[high]; // Choosing the last element as the pivot
        int i = (low - 1); // Index of the smaller element

        for (int j = low; j < high; j++) {
            // If the current element is smaller than or equal to the pivot
            if (values[j] <= pivot) {
                i++; // Increment index of smaller element
                swap(&values[i], &values[j]);
                swap(&indices[i], &indices[j]);
            }
        }
        swap(&values[i + 1], &values[high]);
        swap(&indices[i + 1], &indices[high]);
        return (i + 1); // Return the partitioning index
    }

    void quicksort(int* values, int low, int high) {
        if (low < high) {
            // pi is partitioning index, values[pi] is now at the right place
            int pi = partition(values, low, high);

            // Recursively sort elements before partition and after partition
            quicksort(values, low, pi - 1);
            quicksort(values, pi + 1, high);
        }
    }


};

Quicksorter quicksorter;


void printArray(int arr[], int size) {
    for (int i = 0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");
}


void printPairs(int* indices, int* values, int length) {
    for (int i = 0; i < length; ++i) {
        printf("(%d,%d) ", indices[i], values[i]);

    }
}


/*
int main() {

    int v[100];
    generateRandomArray(v, 100);

    Quicksorter quicksorter(v, 100);

    int k[100]; for (int i = 0; i < 100; ++i) k[i] = i;
    quicksorter.mimic(k, sizeof(int));

    printPairs(k, v, 100);
    /*
    int arr2[100];
    memcpy(arr2, arr, 400);

    int n = 100;

    int indices[100]; for (int i = 0; i < 100; ++i) indices[i] = i;

    printf("Unsorted array: \n");
    printPairs(indices, arr, 100);


    quicksort(arr, 0, 99, indices); // Pass the start and end pointers

    printf("\n\nSorted: \n");
    printPairs(indices, arr, 100);


    printf("counter: %d", counter);

    abc(indices, arr2, 100);

    printf("\n\nabc: \n");
    printPairs(indices, arr2, 100);


    return 0;




}

*/
