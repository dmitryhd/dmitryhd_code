/************************************
 * Semantics: simple acces to all common libs
 scripts, and debug functions, with two
 strokes;
 * Syntax: (cp template.h template.c ./)
 * 	#inlcude "template.h"
 * 	int debug_level;
 * To compile project:
************************************/
#ifndef _TEMPLATE_
#define _TEMPLATE_
//------Libs---------
#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>

#define EARG 1
#define SUCC 0
#define FAIL 1
#define TRUE 1
#define FALSE 0
enum boolean {false,true};
//------Macros-------
#define openf(f,name,mode)\
if((f=fopen(name,mode))==NULL){\
	fprintf(stderr,"file error\n");\
	exit(1);\
}
#define alloc_mem(a,type,num)\
if((a=(type *)realloc(a,(num)*sizeof(type)))==NULL){\
	fprintf(stderr,"mem err\n");\
	exit(2);\
}
//-----Debug--------
extern int debug_level;
void trace(short level, char * format, ...);
#endif
