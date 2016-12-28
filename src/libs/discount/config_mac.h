/*
 * configuration for markdown, generated 2016年11月19日 星期六 04时49分36秒 CST
 * by jingqi@jqmac.local
 */
#ifndef __AC_MARKDOWN_MACOSX_D
#define __AC_MARKDOWN_MACOSX_D 1

#ifdef __APPLE__

#define OS_DARWIN 1
#define USE_DISCOUNT_DL 1
#define while(x) while( (x) != 0 )
#define if(x) if( (x) != 0 )
#define DWORD unsigned int
#define WORD unsigned short
#define BYTE unsigned char
#define HAVE_PWD_H 1
#define HAVE_GETPWUID 1
#define HAVE_SRANDOM 1
#define INITRNG(x) srandom((unsigned int)x)
#define HAVE_BZERO 1
#define HAVE_RANDOM 1
#define COINTOSS() (random()&1)
#define HAVE_STRCASECMP 1
#define HAVE_STRNCASECMP 1
#define HAVE_FCHDIR 1
#define TABSTOP 4
#define PATH_FIND "/usr/bin/find"
#define PATH_SED "/usr/bin/sed"

#define VERSION "2.1.6"

#endif

#endif/* __AC_MARKDOWN_MACOSX_D */
