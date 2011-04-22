#include "template.h"
int debug_level;
void trace(short level, char * format, ...){
	if(level<0 || level > debug_level)
		return;
	va_list args;
	va_start(args,format);
	vprintf(format,args);
	va_end(args);
}
