#ifndef HUFFMAN_H
#define HUFFMAN_H

struct huffman_node {                   /* example         */
        char data;                      /* char o          */
        char bits;                      /* bits of newcode 1-32 */
        unsigned short newcode;         /*                 */
        unsigned int priority;          /* 5               */
        struct huffman_node *lnext;     /* ***             */     
        struct huffman_node *rnext;     /* ***             */
        struct huffman_node *next;      /* ***             */
};

struct tree {
        unsigned char oldcode;
        /*unsigned char bits;*/
        /*unsigned short newcode;*/
        struct tree *rchild;
        struct tree *lchild;
};

struct huffman_tags {
        unsigned char fillbits[4];
        unsigned char magic;
        unsigned char bytes1;
        unsigned char bytes2;
        unsigned char bytes4;
};

#define ALL_CHAR                        0x01 
#define ALL_SHORT                       0x02 
#define ALL_INT                         0x04 
#define MAGIC_MAC                       0x0f
#define HUFFMAN_SORT_BIG_FIRST          0x01
#define HUFFMAN_SORT_SMALL_FIRST        0x02
#define TMP_FILE                        "tmp.huffman"
#define SRC_FILE                        "Screenshot.png"
#define HUFFMAN_HEADER_SIZE             1024
#define HUFFMAN_TAGS_SIZE               8
#define ONE_CHAR                        8
#define ONE_SHORT                       16
#define ONE_INT                         32

struct huffman_header {
        char name[128];         /* test.mp3        */
        
        char size[8];      /* 4,500,000 bytes */
        char mode[8];
        char chksum[8];            /* */
        char version[8];          
        
        char typeflag[32];          /* */
        char password[32];       /* 123 */
        char mtime[32];
        
        unsigned char bits[256];
        unsigned short newcode[256];
        /*char fillbits[256];*/
};

/*extern int huffman_encode(char *encode_s, unsigned int length, struct huffman_header *header);*/
extern int huffman_encode(char *src, unsigned int length, struct huffman_tags *tags);

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
