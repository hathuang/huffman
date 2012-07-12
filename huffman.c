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
static int isnew(struct node *huffman_node, int size, char new)
{
        int i = 0;
        if (!huffman_node) {
                return -1;
        } 
        while (i < size) {
                if (new == huffman_node[i].data) {
                        huffman_node[i].priority++;
                        return 0; // the same
                } 
        }
        huffman_node[size].data = new;
        huffman_node[size].priority++;
        return 1;
}


int huffman_encode(char *encode_s)
{
        char *src = encode_s;
        int len = strlen(src);
        int ret;
        int huffman_size = 0;
        
        if (!src || !len) {
                printf("ulgy input val\n");
                return -1;
        } 
        // get the arr node.data form 0x00-0xff(256 elements) 
/* 
        char huffman_arr[256] = {0};
        int huffman_size = 0;
        int i = 0;
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
*/
        struct node huffman_node[512]; // 256 node

        memset(huffman_node, 0, sizeof(struct node) * 512);

        huffman_node[huffman_size].data = *(src + 0);
        huffman_node[huffman_size].priority++;

        huffman_size++;
        int i = 1;
        while (i < len) {
                ret = isnew(huffman_arr, huffman_size, *(src+i));
                if (ret != -1) {
                        huffman_size += ret;
                } else {
                        return -1;
                }
                ++i;
        }

        // in a Array : huffman_node
        // sort
        
        if (bubble_sort_node(huffman_node, huffman_size)) {
                printf("fail to sort\n");
                return -1;
        } 












        

}
