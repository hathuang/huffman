#ifndef HUFFMAN_H
#define HUFFMAN_H

extern int add(int a, int b);
extern int huffman_encode(char *encode_s);

struct node {               /* example */
        char data;          /* char B  */
        char newcode;        /* 0x22    */
        int priority;       /* 5       */
        struct node *hnext; /* ***     */     
        struct node *lnext; /* ***     */     
        struct node *rnext; /* ***     */
};

struct line {
        char ch;
        int priority;
};

#endif // huffman.h
