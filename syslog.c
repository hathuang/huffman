#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include "syslog.h"

#define LOG_FILE_MAX_SIZE       (4 << 20) /* 4M */
#define DEBUG_LEVEL             6

int init_syslog()
{
        int fd;
	if(access(LOG_FILE,F_OK | R_OK | W_OK)) {
		if((fd = creat(LOG_FILE,0777)) < 0) {
			return -1;
		} else {
                        close(fd);
                }
	} else {
		return truncate(LOG_FILE, 0);
        }

	return 0;
}

int syslog(int priority, const char *_format,...)
{
	int pri = priority & 0x00000007;
	int fac = priority & 0x00000078;
	
        if(DEBUG_LEVEL < pri) return 0;
	
        time_t timestamp;
	char _timestamp[21] = {0};	
	time(&timestamp);
	struct tm * _tm = localtime(&timestamp);
		
        FILE *_fp = fopen(LOG_FILE,"a+");
        long length;

        if (!_fp) return -1;
        if (fseek(_fp, 0, SEEK_END)) return -1;
        if ((length = ftell(_fp)) >= LOG_FILE_MAX_SIZE) {
                if (truncate(LOG_FILE, 0) || fseek(_fp, 0, SEEK_SET)) {
                        fclose(_fp);
                        return -1;
                }
                syslog(LOG_USER | LOG_WARNING, "The log file(size:%ld) is larger than the LOG_FILE_MAX_SIZE(%ld) !",
                        length, LOG_FILE_MAX_SIZE); 
        }
	sprintf(_timestamp, "%04d-%02d-%02d %02d:%02d:%02d",
                _tm->tm_year+1900, _tm->tm_mon+1, _tm->tm_mday,
                _tm->tm_hour, _tm->tm_min, _tm->tm_sec);
        fprintf(_fp, "%s ", _timestamp);
			
        switch(fac) {
        case LOG_USER:
                fprintf(_fp,"USER ");
                break;
        case LOG_SYSTEM:
                fprintf(_fp,"SYSTEM ");
                break;
        default:
                fprintf(_fp,"UNKOWN ");
                break;
        }

        switch(pri) {
                case LOG_ERR:
                        fprintf(_fp,"ERROR ");
                        break;
                case LOG_WARNING:
                        fprintf(_fp,"WARNING ");
                        break;
                case LOG_INFO:
                        fprintf(_fp,"INFO ");
                        break;
                case LOG_DEBUG:
                        fprintf(_fp,"DEBUG ");
                        break;
                default :
                        fprintf(_fp,"UNKNOWN ");		
        }

        va_list _args;
        va_start(_args,_format);
        vfprintf(_fp,_format,_args);
        va_end(_args);
        fprintf(_fp,"\r\n");
        fclose(_fp);

        return 0;
}
