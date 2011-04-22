//here we try to find out - how many recusively sumlinks can be created?
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#define BUF 0x100

int main( int argc, char *argv[])
{
	char oldpath[BUF], newpath[BUF], tmp[BUF]; 
	//Путь к исходному файлу и создаваемой ссылке
	//tmp &tmp2 for - number
	if( argc != 2){
		printf("usage: task_9 JUSTFILE\n");
		return 1;
	}
	unsigned long i; //наш счетчик
	int ret = 0;
	int fd;

	strcpy( oldpath, argv[1]);
	strcpy( newpath, oldpath);
	sprintf( tmp, "%08d", 0);
	strcat( newpath, tmp);
	printf("try simlink %s to %s, 1 depth\n", oldpath, newpath);
	ret = symlink (oldpath, newpath);
	strcpy( oldpath, newpath);
	//до тех пор, пока симлинк успешен:
	for( i = 1; ret == 0 ; i++){ 
		strcpy( newpath, argv[1]);
		sprintf( tmp, "%08ld", i);
		strcat( newpath, tmp);
		//just debug
		printf("try simlink %s to %s, %ld depth\n", oldpath, newpath, i);
		ret = symlink (oldpath, newpath);
		//perror("er:\n");
		fd = open(newpath, O_RDWR | O_CREAT, 0666);
	        if( fd == -1){
			perror("final link\n");
			break;
		}	
		strcpy( oldpath, newpath);
	}
	printf("%ld recursively symlinks \n", i);
	return 0;
}
