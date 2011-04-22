/*Written by: Khodakov Dmitry 817gr, 23.12.09 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>

// Реализация: 
// Программа получает очередь сообщений, отпочковывает 3х "рабочих" 
// каждый из которых должен получить сигнал о том, что деталь поступила
// затем присоединить свою часть, отписаться об этом в очередь.
// Родительский процесс ждет, пока отпишутся остальные и "ставит" новую деталь.

// типы сообщений соответсвуют действиям
#define NEW_DETAIL_FOR_1 3
#define NEW_DETAIL_FOR_2 4
#define NEW_DETAIL_FOR_3 5
#define ONE_NIPPLE_INSTALLED 1
#define ONE_SCREW_INSTALLED 2
// число рабочих
#define NUM_OF_WORKERS 3

// сообщение NEW_DETAIL_FOR_(n) - сообщение о посплении новой детали для н-ного рабочего.
// Это сделано чтобы предотвратить такую ситуацию: (если бы был один NEW_DETAIL. далее нд)
// 1й гайкокрут быстр, (1с)
// 2й тоже (1с)
// а вот винтоверт тормозит (50 с)
// тогда 1й и 2й гайкокруты "вычерпают" все сообщения нд из очереди.
// и винтоверту ничего не достанется. А т.к. гл. программа поместила только 3 сообщения
// в очередь, то винтоверт будет простаивать и гл. программа будет его ждать "deadlock"

// Сообщение.
// mtype - тип сообщения
// mtext - сообщение, я его не использую, но присутсвовать оно должно.
typedef struct msgbuf {
	long mtype;
	char mtext[1]; // Затычка
} message;

void create_nipple_worker( int n);
void create_screw_worker( int n);
void create_queue ();


// id очереди.
int msqid;

int main (int argc, char **argv)
{
	if ( argc != 2){
		printf ("usage: %s number_of_details\n", argv[0]);
		return 2;
	} 
	// получаем очередь сообщений для всех процессов.
	create_queue();
	/* создаем процесс - закрутчик гаек. */
	create_nipple_worker( 0);
	create_nipple_worker( 1);
	/* создаем процесс - закрутчик винтов. */
	create_screw_worker( 2);
	message mbuf;
	// cnt раз подадим деталь на тиски.
	int cnt = atoi (argv[1]);
	printf (" %d details\n", cnt);
	do{
		//сообщим всем процессам - что деталь установлена. 
		//(поместим _три_ сообщения в очередь)
		int i;
		printf("send new detail, last #%d\n", cnt-1);
		for( i = 0; i < NUM_OF_WORKERS; i++){
			message mbuf;
			mbuf.mtype = NEW_DETAIL_FOR_1 + i;
			//new_detail_for_1
			//new_detail_for_2
			//new_detail_for_3 =)
			if ( msgsnd ( msqid, &mbuf, 2, IPC_NOWAIT) < 0){
				perror ("msgsnd");
				return 1;
			}
		}
		// Проверим, все-ли закончили свою работу?
		if ( msgrcv (msqid, &mbuf, 2, ONE_NIPPLE_INSTALLED, 0) < 0 ){
			perror ("msgrcv");
			return 1;
		}
		if ( msgrcv (msqid, &mbuf, 2, ONE_NIPPLE_INSTALLED, 0) < 0 ){
			perror ("msgrcv");
			return 1;
		}
		if ( msgrcv (msqid, &mbuf, 2, ONE_SCREW_INSTALLED, 0) < 0 ){
			perror ("msgrcv");
			return 1;
		}
	} while (--cnt > 0);
	
	//удаляем очередь
	if ( msgctl (msqid, IPC_RMID, NULL) < 0 ){
		perror ("queue delete:");
		return 1;
	}
	printf("finished\n");

	return 0;
}

/* создаем процесс - закрутчик гаек. */
// n - номер рабочего (от 0)
void create_nipple_worker( int n)
{
	pid_t pid = fork();
	if ( pid == 0) // процесс-ребенок - установщик гаек.
	{
		printf ("Nipple worker created\n");
		while (1) {
			// проверяем, поступила ли новая деталь 
			message mbuf;
			if ( msgrcv (msqid, &mbuf, 2, NEW_DETAIL_FOR_1 + n, 0) < 0 ){
				if ( errno != EIDRM) perror ("msgrcv");
				exit(1);
			}
			// рапортуем об установке своей части на деталь
			printf(" nipple installed\n");
			mbuf.mtype = ONE_NIPPLE_INSTALLED;
			if ( msgsnd (msqid, &mbuf, 2, IPC_NOWAIT) < 0){
				perror ("msgsnd");
				exit(1);
			}
		}
	}
	//родительский процесс возвращается в main
	return ;
}

/* создаем процесс - закрутчик винтов. */
void create_screw_worker( int n)
{
	pid_t pid = fork();
	if ( pid == 0) // процесс-ребенок - установщик винтов.
	{
		printf ("Screw worker created\n");
		while (1) {
			message mbuf;
			sleep(3);
			if ( msgrcv (msqid, &mbuf, 2, NEW_DETAIL_FOR_1 + n, 0) < 0 ){
				if ( errno != EIDRM) perror ("msgrcv");
				exit(1);
			}
			printf(" screw installed  \n");
			mbuf.mtype = ONE_SCREW_INSTALLED;
			if ( msgsnd (msqid, &mbuf, 2, IPC_NOWAIT) < 0){
				perror ("msgsnd");
				exit(1);
			}
		}
	}
	//родительский процесс возвращается в main
	return ;
}

/* Получение очереди. */
//Создаем уникальный ключ из имени исполняемого файла и генерируем id очереди в глобальн.пер. msqid
void create_queue ()
{
	int msflg = IPC_CREAT | 0666; // аттрибуты новой очереди (ipc_creat - создать очередь, если таковой не имеется)
	key_t key;
	if( (key = ftok ("detail_assembling", 0)) < 0 ){ // произвольный ключ.
		perror ("key");
		exit(1);
	}
	if ( (msqid = msgget (key, msflg )) < 0){
		perror ("msgget");
		exit(1);
	}
}
