#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "huffman.h"
#include "sort.h"

#define ARR_SIZE 32


int main(int argc, char *argv[])
{
        char arr[ARR_SIZE];
        int i;

        printf("hello world !\n");
        printf("2+3=%d\n", add(2, 3));
       
        i = 0;
        srand(time(0));
        while (i < ARR_SIZE) {
                *(arr + i++) = 0xff & rand();
        }
        i = 0;
        while (i < ARR_SIZE) {
                printf("arr[%04d] = %04d\n", i, *(arr+i));
                ++i;
        }
        if (bubble_sort(arr, ARR_SIZE)) {
                printf("fail to bubble_sort\n");
        }
        printf("After bubble_sort=========================\n");
        printf("After bubble_sort=========================\n");
        i = 0;
        while (i < ARR_SIZE) {
                printf("arr[%04d] = %04d\n", i, *(arr+i));
                ++i;
        }


        
        return 0;
}
