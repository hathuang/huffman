#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <time.h>
#include "syslog.h"

#define LOG_FILE  "./log.txt"
#define _DEBUG_LEVEL              "DEBUG_LEVEL"

int DebugLevel = 7;

int init_syslog()
{
	if(access(LOG_FILE,F_OK | R_OK | W_OK))
	{
                int fd;
		if(-1 == (fd = creat(LOG_FILE,0777)))
		{
			return -1;
		}else {
                        close(fd);
                }

	}else{
		truncate(LOG_FILE,0);
        }

	return 0;
}

/*
 * How to use: export MA_LOG_LEVEL=7
 * */
int Syslog(int priority,const char *_format,...)
{
	int pri=priority&0x00000007;
	int fac=priority&0x00000078;
	int level;
	/*
	char *debug_level=getenv("MA_LOG_LEVEL");
	if(!debug_level)
	{
		setenv("MA_LOG_LEVEL","6",0);
		level=6;
	}
	else
	{
		level=atoi(debug_level);
	}
	if(level<pri)
		return 0;
	
	if(DebugLevel < pri)
		return 0;
	*/
	level = 7;
	time_t timestamp;
	char _timestamp[21]={0};	
	time(&timestamp);
	struct tm * _tm=localtime(&timestamp);
	sprintf(_timestamp,"%04d/%02d/%02d-%02d:%02d:%02d",
		_tm->tm_year + 1900,
		_tm->tm_mon + 1,
		_tm->tm_mday,
		_tm->tm_hour,
		_tm->tm_min,
		_tm->tm_sec);
		
		FILE *_fp=fopen(LOG_FILE,"a+");
		if(!_fp)
			return -1;
		
		fprintf(_fp, "%s ", _timestamp);
			
		switch(fac)
		{
			case LOG_USER:
				fprintf(_fp,"USER ");
				break;
			case LOG_SYSTEM:
				fprintf(_fp,"SYSTEM ");
				break;
			default:
				fprintf(_fp,"UNKOWN ");
		}
	
		switch(pri)
		{
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
