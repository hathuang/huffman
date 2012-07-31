#ifndef HUFFMAN_H
#define HUFFMAN_H

extern int add(int a, int b);
extern int huffman_encode(char *encode_s);

struct huffman_node {                   /* example         */
        char data;                      /* char o          */
        char bits;                      /* bits of newcode */
        char newcode;                    /*                 */
        unsigned int priority;                   /* 5               */
        struct huffman_node *lnext;     /* ***             */     
        struct huffman_node *rnext;     /* ***             */
        struct huffman_node *next;      /* ***             */
};

#define HUFFMAN_SORT_BIG_FIRST          0x01
#define HUFFMAN_SORT_SMALL_FIRST        0x02

#endif // huffman.h
