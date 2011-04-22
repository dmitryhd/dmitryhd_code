#ifndef _MY_TEMPLATE_
#define _MY_TEMPLATE_
//------Libs---------
#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <ctype.h>
//------Macros-------
#define openf(f,name,mode)\
if((f=fopen(name,mode))==NULL){\
	fprintf(stderr,"can't open file %s\n",name);\
	exit(1);\
}
#define alloc_mem(a,type,num)\
if((a=(type *)malloc((num)*sizeof(type)))==NULL){\
	fprintf(stderr,"mem err\n");\
	exit(2);\
}
//-----Debug--------
extern int debug_level;
#endif
