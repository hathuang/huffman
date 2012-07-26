#ifndef HUFFMAN_H
#define HUFFMAN_H

extern int add(int a, int b);
extern int huffman_encode(char *encode_s);

struct huffman_node {                   /* example         */
        char data;                      /* char o          */
        char bits;                      /* bits of newcode */
        char newcode                    /*                 */
        int priority;                   /* 5               */
        struct huffman_node *lnext;     /* ***             */     
        struct huffman_node *rnext;     /* ***             */
        struct huffman_node *next;      /* ***             */
};
/*
struct huffman_line {
        int priority;
        struct huffman_node *node;
        struct huffman_line *next;
};
*/
#endif // huffman.h
