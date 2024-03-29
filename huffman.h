#ifndef HUFFMAN_H
#define HUFFMAN_H

struct huffman_node {                   /* example         */
        char data;                      /* char o          */
        char bits;                      /* bits of newcode */
        unsigned short newcode;         /*                 */
        unsigned int priority;          /* 5               */
        struct huffman_node *lnext;     /* ***             */     
        struct huffman_node *rnext;     /* ***             */
        struct huffman_node *next;      /* ***             */
};

struct tree {
        unsigned char oldcode;
        struct tree *rchild;
        struct tree *lchild;
};

struct huffman_tags {
        unsigned char fillbits[2];
        unsigned char password[2];
        unsigned char magic;
        unsigned char bytes1;
        unsigned char bytes2;
        unsigned char bytes4;
};

/*#define HUFFMAN_FILE_HEADER             "Huff"*/
#define HUFFMAN_FILE_HEADER             "Hu"
#define ALL_CHAR                        0x01 
#define ALL_SHORT                       0x02 
#define ALL_INT                         0x04 
#define MAGIC_MAC                       0x0f
#define HUFFMAN_SORT_BIG_FIRST          0x01
#define HUFFMAN_SORT_SMALL_FIRST        0x02
#define HUFFMAN_HEADER_SIZE             1024
#define HUFFMAN_PWD                     (1<<1)
#define HUFFMAN_NULL_PWD                (1<<0)
#define HUFFMAN_TAGS_SIZE               (sizeof(struct huffman_tags)) 
#define ONE_CHAR                        8
#define ONE_SHORT                       16
#define ONE_INT                         32

#define SWAP(x, y)      do {\
                                (x) = (x) ^ (y);\
                                (y) = (x) ^ (y);\
                                (x) = (x) ^ (y);\
                        } while (0)

extern int huffman_compression(const char *outfile, char *src, unsigned int length, struct huffman_tags *tags);
extern int huffman_decompression(const char *infile, const char *outfile);

/*
                huffman file arch

-------------------------------------------------
|                                               |
|             huffman_tags(8 bytes)             |
|                                               |
|------------------------------------------------
|         |         |                           |
| 1 byte  | 1byte   |   2 bytes(as tags say)    |
|         |         |                           |
|---------+---------+----------------------------
|         |         |                           |
| oldcode | bits    | newcode                   |
|         |         |                           |
|------------------------------------------------
|                                               |
|                file body                      |
|                                               |
-------------------------------------------------
*/
#endif // huffman.h
