#ifndef SATLOG_H
#define SATLOG_H

#include <features.h>
#define __need___va_list
#include <stdarg.h>

#define	LOG_EMERG	0	/* system is unusable */
#define	LOG_ALERT	1	/* action must be taken immediately */
#define	LOG_CRIT	2	/* critical conditions */
#define	LOG_ERR		3	/* error conditions */
#define	LOG_WARNING	4	/* warning conditions */
#define	LOG_NOTICE	5	/* normal but significant condition */
#define	LOG_INFO	6	/* informational */
#define	LOG_DEBUG	7	/* debug-level messages */

#define	LOG_PRIMASK	0x07	/* mask to extract priority part (internal) */

#define LOG_SYSTEM		(0x01 << 4)	/**< system log message */
#define LOG_USER		(0x02 << 4)	/**< user log */
#define LOG_DEVICE		(0x03 << 4)	/**< device or pc output by expect */
#define LOG_EXPECT		(0x04 << 4)	/**< EXPECT log */
#define LOG_RESULT		(0x05 << 4)	/**< output */
#define LOG_IMPORTANT	        (0x10 << 4)	/**< output to syslog */
#define LOG_FILE                "./log.txt"
extern int init_syslog();
extern int syslog(int priority, const char * _format,...);
#endif
