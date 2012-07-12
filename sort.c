#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int quick_sort(char *arr, int num)
{
        return 0;
}

int bubble_sort(char *arr, int num)
{
        int i, j;

        if (arr == NULL || num < 0) {
                return -1; 
        }
        if (num <= 1) {
                return 0;
        } 

        for (i = 0; i < num - 1; i++) {
                for (j = i + 1; j < num; j++) {
                        if (*(arr + i) > *(arr + j)) {
                                *(arr + j) = *(arr + j) ^ *(arr + i);
                                *(arr + i) = *(arr + j) ^ *(arr + i);
                                *(arr + j) = *(arr + j) ^ *(arr + i);
                        } 
                }
        }
        return 0;
}
