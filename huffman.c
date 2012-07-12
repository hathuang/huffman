/*
http://dungenessbin.diandian.com/post/2012-05-23/21949784
*/
#include <stdio.h>
#include <string.h>

#include "huffman.h"
#include "sort.h"

int add(int a, int b)
{
        return a + b;
}

// huffman encode
static int isnew(char *arr, int size, char new)
{
        int i = 0;
        if (!arr) {
                return -1;
        } 
        while (i < size) {
                if (new == *(arr + i++)) {
                        return 0; // the same
                } 
        }
        *(arr + size) = new;
        return 1;
}


int huffman_encode(char *encode_s)
{
        char *src = encode_s;
        int len = strlen(src);
        
        if (!src || !len) {
                printf("ulgy input val\n");
                return -1;
        } 
        // get the arr node.data form 0x00-0xff(256 elements) 
        char huffman_arr[256] = {0};
        int huffman_size = 0;
        int i = 0;
        int ret;
        
        huffman_arr[i] = *(src + 0);
        ++huffman_size;

        while (i < len) {
                ret = isnew(huffman_arr, huffman_size, *(src+i));
                if (ret != -1) {
                        huffman_size += ret;
                } else {
                        return -1;
                }
                ++i;
        }

        // in a line
        

}




