#ifndef HUFFMAN_H
#define HUFFMAN_H

struct huffman_node {                   /* example         */
        char data;                      /* char o          */
        char bits;                      /* bits of newcode 0-8 */
        char newcode;                   /*                 */
        unsigned int priority;          /* 5               */
        struct huffman_node *lnext;     /* ***             */     
        struct huffman_node *rnext;     /* ***             */
        struct huffman_node *next;      /* ***             */
};

#define HUFFMAN_SORT_BIG_FIRST          0x01
#define HUFFMAN_SORT_SMALL_FIRST        0x02
#define TMP_FILE                        "tmp.huffman"
#define SRC_FILE                        "Screenshot.png"
#define ONE_CHAR                        8
#define HUFFMAN_HEADER_SIZE             1024

struct huffman_header {
        char name[128];         /* test.mp3        */
        
        char size[8];      /* 4,500,000 bytes */
        char mode[8];
        char chksum[8];            /* */
        char version[8];          
        
        char typeflag[32];          /* */
        char password[32];       /* 123 */
        char mtime[32];
        
        char bits[256];
        char newcode[256];
        char fillbits[256];
};

struct tar_header {
        char name[100];
        char mode[8];
        char uid[8];
        char gid[8];
        char size[12];
        char mtime[12];
        char chksum[8];
        char typeflag;
        char linkname[100];
        char magic[6];
        char version[2];
        char uname[32];
        char gname[32];
        char devmajor[8];
        char devminor[8];
        char prefix[155];
        char padding[12];
};

extern int huffman_encode(char *encode_s, unsigned int length, struct huffman_header *header);

#endif // huffman.h
