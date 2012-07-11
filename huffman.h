#ifndef HUFFMAN_H
#define HUFFMAN_H

extern int add(int a, int b);
extern int huffman_encode(char *encode_s);

struct node {
        char data;
        struct node *lnext;      
        struct node *rnext;
};

struct line {
        char ch;
        int priority;
};

#endif // huffman.h
