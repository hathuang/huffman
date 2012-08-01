#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "huffman.h"
#include "sort.h"
#include "syslog.h"
#include <string.h>

#ifdef Debug
//#define syslog(x) Syslog(x) 
//#define init_syslog(x) init_Syslog(x) 
#else
//#define syslog(x) do{}while(0)
//#define init_syslog(x)
#endif

int main(int argc, char *argv[])
{
        char buf[64] = "\"beepboopbeer!\"";
        init_syslog();        
        printf("hello world !\n");
        printf("2+3=%d\n", add(2, 3));
        printf("\n\n");


        printf("start to huffman\n");
        printf("sizeof (struct huffman_header) = %d\n", sizeof (struct huffman_header));
        if (huffman_encode(buf, strlen(buf))) {
                printf("error to huffman\n");
        } 
        printf("Good !\n");
        
        return 0;
}
