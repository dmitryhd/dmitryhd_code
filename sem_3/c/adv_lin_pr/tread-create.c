#include <stdio.h>
#include <pthread.h>

void * thread(void * s){
	while (1)
		fputc('a',stderr);
	return NULL;;
}

int main(int argc, char *argv[]){
	pthread_t threadid;
	pthread_create(&threadid,NULL,&thread,NULL);
	while (1)
		fputc('b',stderr);
	return 0;
}
