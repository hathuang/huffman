#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include "huffman.h"
#include "syslog.h"

#ifdef Debug
//#define syslog(x) Syslog(x) 
//#define init_syslog(x) init_Syslog(x) 
#else
//#define syslog(x) do{}while(0)
//#define init_syslog(x)
#endif

int main(int argc, char *argv[])
{
        //char buf[64] = "beep boop beer!";
        //char header_buf[1024];
        char tags_buf[8];
        int len, ret;
        struct huffman_tags *tags = NULL;
        init_syslog();        

        printf("start to huffman\n");
        printf("sizeof (struct huffman_header) = %d\n", sizeof(struct huffman_header));
        printf("sizeof (struct huffman_tags) = %d\n", sizeof(struct huffman_tags));
        
        //struct huffman_header *header = (struct huffman_header *)header_buf;
        tags = (struct huffman_tags *)tags_buf;

        // init the huffman_header
        //memset(header_buf, 0, sizeof(header_buf));
        //strncpy(header->name, SRC_FILE, strlen(SRC_FILE));
        //strncpy(header->name, "hello", 5);
        //strncpy(header->version, "1.0", strlen("1.0"));
        strncpy((char *)(tags->fillbits), "Huff", 4);
        tags->magic = 0;
        tags->bytes1 = 0;
        tags->bytes2 = 0;
        tags->bytes4 = 0;
                
        // get buf of the SRC_FILE 
        char *_buf = NULL;

        FILE *fp = fopen(SRC_FILE, "r"); 
        if (!fp) {
                perror("Fail to fopen SRC_FILE");
                return 0;
        }

        fseek(fp, 0, SEEK_END);
        long file_len = ftell(fp);
        fclose(fp);
        //header->size[0] = (file_len >> 0) & 0xff;
        //header->size[1] = (file_len >> 8) & 0xff;
        //header->size[2] = (file_len >> 16) & 0xff;
        //header->size[3] = (file_len >> 24) & 0xff;
        //file_len = 15; 
        printf("file length = %ld\n", file_len);
        _buf = (char *)malloc(file_len * (sizeof(char)));
        if (_buf == NULL) {
                perror("Fail to malloc for _buf");
                return 0;
        }
        int fd = open(SRC_FILE, O_RDWR);
        if (fd < 0) {
                perror("Fail to open SRC_FILE.");
                return 0;
        }
        len = 0;
        while (len < file_len) {
                ret = read(fd, _buf+len, file_len-len);
                if (ret <= 0) {
                        close(fd);
                        perror("Fail to read SRC_FILE.");
                        return 0;
                }
                len += ret;
        }
        //if (*(_buf + file_len - 1) == '\n') {
        //*(_buf + (--file_len)) = 0;    
        //} 

        close(fd);
        //if (huffman_encode(_buf, file_len, header)) {
        if (huffman_encode(_buf, file_len, tags)) {
                printf("error to huffman\n");
        }
        free(_buf);
        printf("Good !\n");
        
        return 0;
}
