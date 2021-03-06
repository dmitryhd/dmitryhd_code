/************************************
 * Semantics: simple acces to all common libs
 scripts, and debug functions, with two
 strokes;
 * Syntax: (cp template.h template.c ./)
 * 	#inlcude "template.h"
 * 	int debug_level;
 * To compile project:
 * NEED WRITE...
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
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/un.h>

#define EARG 1
#define ESOCK 20
#define ECONN 21
#define EBIND 22
#define SUCC 0
//------Macros-------
#define openf(f,name,mode)\
do {\
	if((f=fopen(name,mode))==NULL){\
		fprintf(stderr,"file error\n");\
		exit(1);\
	}\
}\
while (0);
#define alloc_mem(a,type,num)\
do {\
	if((a=(type *)realloc(a,(num)*sizeof(type)))==NULL){\
		fprintf(stderr,"mem err\n");\
		exit(2);\
	}\
}\
while (0);
//-----Debug--------
extern int debug_level;
void trace(short level, char * format, ...);
#endif
