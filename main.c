#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include "huffman.h"
#include "syslog.h"

#define VERSION                 "1.0"
#define Author                  "Hat Huang"

#define MODE_TIME               (0x01 << 1)
#define MODE_PASSWORD           (0x01 << 2)
#define MODE_DELETE             (0x01 << 3)
#define MODE_COMPRESSION        (0x01 << 5)
#define MODE_DECOMPRESSION      (0x01 << 7)

int usage(const char *arg)
{
        fprintf(stderr, "\n     Author  : %s", Author);
        fprintf(stderr, "\n     Version : %s\n\n", VERSION);
        fprintf(stderr, "Usage : %s [OPTION...] [INPPUT FILE] [OUTPUT FILE]\n", arg);
        fprintf(stderr, "\nMain operation mode:\n\n");
        fprintf(stderr, "       -c [compression]\n");
        fprintf(stderr, "       -x [decompression]\n");
        fprintf(stderr, "       -d [delete input file]\n");
        fprintf(stderr, "       -t [time]\n");
        fprintf(stderr, "       -p [password]\n");
        fflush(stderr);
        return 0;
}

int main(int argc, char *argv[])
{
        char tags_buf[sizeof(struct huffman_tags)];
        int len, ret, fd, mode;
        struct huffman_tags *tags = NULL;
        char *buf = NULL, *infile = NULL, *outfile = NULL;
        unsigned int file_len;
        struct timeval tv_start, tv_end;

        if (argc != 4 || argv[1][0] != '-' || (len = strlen(argv[1])) <= 1) return usage(argv[0]);
        mode = 0;
        ret = 1; // skip '-'
        while (ret < len) {
                switch (argv[1][ret++]) {
                case 'c':
                        if (mode & MODE_DECOMPRESSION) return usage(argv[0]);
                        mode |= MODE_COMPRESSION;
                        break;
                case 'x':
                        if (mode & MODE_COMPRESSION) return usage(argv[0]);
                        mode |= MODE_DECOMPRESSION;
                        break;
                case 'd':
                        mode |= MODE_DELETE;
                        break;
                case 't':
                        mode |= MODE_TIME;
                        break;
                case 'p':
                        mode |= MODE_PASSWORD;
                        break;
                default :
                        return usage(argv[0]);
                        break;
                }
        }
        infile = argv[2];
        outfile = argv[3];

        if ((mode & MODE_TIME) && gettimeofday(&tv_start, NULL)) {
                perror("fail to do gettimeofday");
                return -1; 
        }
        init_syslog();
        if (mode & MODE_COMPRESSION) {
                // Compression
                tags = (struct huffman_tags *)tags_buf;
                strncpy((char *)(tags->fillbits), HUFFMAN_FILE_HEADER, sizeof(tags->fillbits));
                if (mode & MODE_PASSWORD)
                        tags->password[0] = HUFFMAN_PWD;
                else
                        tags->password[0] = HUFFMAN_NULL_PWD;
                tags->password[1] = 0;
                tags->magic = ONE_CHAR << 4;
                tags->bytes1 = 0;
                tags->bytes2 = 0;
                tags->bytes4 = 0;
                        
                if ((fd = open(infile, O_RDONLY)) < 0
                        || (file_len = lseek(fd, 0, SEEK_END)) <= 0
                        || lseek(fd, 0, SEEK_SET) < 0) {
                        perror("Fail to open SRC_FILE.");
                        return 0;
                }

                if (!(buf= (char *)malloc(file_len * (sizeof(char))))) {
                        perror("Fail to malloc for buf");
                        return 0;
                }

                len = 0;
                while (len < file_len) {
                        if ((ret = read(fd, buf+len, file_len-len)) <= 0) {
                                close(fd);
                                perror("Fail to read SRC_FILE.");
                                return 0;
                        }
                        len += ret;
                }
                close(fd);
                if (huffman_compression(outfile, buf, file_len, tags)) {
                        free(buf);
                        printf("Error to huffman\n");
                        return -1;
                }
                free(buf);
        } else if (mode & MODE_DECOMPRESSION) {
                // Decompression 
                if (huffman_decompression(infile, outfile)) {
                        syslog(LOG_USER | LOG_ERR , "%s : Fail to huffman_decompression", __func__);
                        return -1;
                }
        }
#ifdef Debug
        printf("Good !\n");
#else
        unlink(LOG_FILE);
#endif
        if (mode & MODE_DELETE) unlink(infile);
        if (mode & MODE_TIME) {
                if (gettimeofday(&tv_end, NULL)) {
                        perror("\nHuffman done, But fail to get time.");
                } else {
                        if (tv_end.tv_usec < tv_start.tv_usec) {
                                tv_end.tv_usec = 1000000 + tv_end.tv_usec - tv_start.tv_usec;
                                tv_end.tv_sec -= tv_start.tv_sec + 1; 
                        } else {
                                tv_end.tv_usec -= tv_start.tv_usec;
                                tv_end.tv_sec -= tv_start.tv_sec; 
                        }
                        fprintf(stderr, "\nHuffman done time : %us %ums\n", tv_end.tv_sec, (tv_end.tv_usec)/1000);
                        fflush(stderr);
                }
        }

        return 0;
}
