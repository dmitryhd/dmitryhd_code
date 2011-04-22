//30.01.10 - Dmitry Khodakov

#include <stdio.h>
#include <string.h>
#include <math.h>

#define BASE 10

//here we have simple implementation of Rabin Karp algorithm for searching substrings 
//in string which is using hashes;
//for exmaple with base = 10: "1234" = int 1234;

void hash ( char * str, int n, unsigned long * h)
{
	int i;
	if ( *h == 0){
		for ( i = n - 1; i >=0; i-- )
			*h += pow(BASE,i)*(str[n -1-i]-'1'+1);
		printf("hashing: %s -> %6lu\n", str, *h);
		return;
	}
	printf("previous hash=%6lu\n",*h);
	if ( *h != 0){
		*h %= (long unsigned)pow(BASE,n-1);
		*h *= BASE;
		*h += str[n-1]-'1'+1;
	}
	printf("hashing: %s -> %6lu\n", str, *h);
}

int RabinKarpSearch ( char * s, int s_len, char * ssub, int ssub_len)
{
	unsigned long s_hash = 0, ssub_hash = 0;
	hash ( ssub, ssub_len, &ssub_hash );
	hash ( s, ssub_len, &s_hash );
	int i;
	char * s_ptr = s;
	for ( i = 0; i <= s_len - ssub_len; i++ ){
		if (s_hash == ssub_hash){
			if(strncmp ( s_ptr, ssub, ssub_len) == 0)
				return i;
		}
		s_ptr++;
		hash ( s_ptr, ssub_len, &s_hash );
	}
	return -1;
}


int main ( int argc, char ** argv) 
{
	if ( argc != 3){
		printf("usage: %s [string] [substring]\n", argv[0]);
		return 1;
	}
	int s_len = strlen (argv[1]);
	printf("string len = %d\n", s_len);
	int ssub_len = strlen (argv[2]);
	printf("substring len = %d\n", ssub_len);
	printf("answer:%d\n", RabinKarpSearch( argv[1], s_len, argv[2], ssub_len));
	return 0;
}
