#ifndef HUFFMAN_H
#define HUFFMAN_H

extern int add(int a, int b);
extern int huffman_encode(char *encode_s);

struct huffman_node {               /* example */
        char data;                  /* char B  */
        char newcode;               /* 0x22    */
        int priority;               /* 5       */
        struct huffman_node *lnext; /* ***     */     
        struct huffman_node *rnext; /* ***     */
};

struct huffman_line {
        /*char ch;*/
        struct huffman_node *curr; /* ***     */     
        struct huffman_line *next; /* ***     */     
        /*int priority;*/
};

#endif // huffman.h
