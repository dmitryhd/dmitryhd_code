#include <stdio.h>
#include <pthread.h>

void * thread (void * unused)
{
	printf("%c\n",'x');
	return NULL;
}

int main()
{
	pthread_t thread_id;
	pthread_create (&thread_id, NULL, &thread, NULL);
	printf("%c\n",'o');
	return 0;
}
