#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "huffman.h"
#include "sort.h"
#include "syslog.h"


int main(int argc, char *argv[])
{
        char buf[64] = "\"beepboopbeer!\"";
        init_syslog();        
        printf("hello world !\n");
        printf("2+3=%d\n", add(2, 3));
        printf("\n\n");


        printf("start to huffman\n");
        if (huffman_encode(buf)) {
                printf("error to huffman\n");
        } 
        printf("Good !\n");
        
        return 0;
}
