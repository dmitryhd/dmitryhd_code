#include <stdio.h>

struct _num
{
	int ch;
	int zn;
};
typedef struct _num num;
//y, x
num arr[3][3];
/*
				x2zn = arr[i][x2[0]].zn * arr[j][x2[1]].zn * arr[k][x2[2]].zn ;
				x2ch = arr[i][x2[0]].ch * arr[j][x2[1]].ch * arr[k][x2[2]].ch ;
				x3zn = arr[i][x3[0]].zn * arr[j][x3[1]].zn * arr[k][x1[2]].zn ;
				x3ch = arr[i][x3[0]].ch * arr[j][x3[1]].ch * arr[k][x3[2]].ch ;
				*/
int main()
{
arr[0][0].ch = 8;
arr[0][0].zn = 9;
arr[0][1].ch = 1;
arr[0][1].zn = 9;
arr[0][2].ch = 0;
arr[0][2].zn = 0;
arr[1][0].ch = 0;
arr[1][0].zn = 0;
arr[1][1].ch = 8;
arr[1][1].zn = 9;
arr[1][2].ch = 1;
arr[1][2].zn = 9;
arr[2][0].ch = 1;
arr[2][0].zn = 18;
arr[2][1].ch = 1;
arr[2][1].zn = 18;
arr[2][2].ch = 8;
arr[2][2].zn = 9;
	int y[3];
	int i, j, k;
	int x1[3] = {0, 1, 0};
	int x2[3] = {0, 0, 2};
	int x3[3] = {1, 2, 2};
	int x1zn, x2zn, x3zn, x1ch, x2ch, x3ch;
	
	for(i = 0; i < 3; i++)
		for(j = 0; j < 3; j++)
			for(k = 0; k < 3; k++){
x1zn = arr[i][x1[0]].zn * arr[j][x1[1]].zn * arr[k][x1[2]].zn ;
x1ch = arr[i][x1[0]].ch * arr[j][x1[1]].ch * arr[k][x1[2]].ch ;
x2zn = arr[i][x2[0]].zn * arr[j][x2[1]].zn * arr[k][x2[2]].zn ;
x2ch = arr[i][x2[0]].ch * arr[j][x2[1]].ch * arr[k][x2[2]].ch ;
x3zn = arr[i][x3[0]].zn * arr[j][x3[1]].zn * arr[k][x3[2]].zn ;
x3ch = arr[i][x3[0]].ch * arr[j][x3[1]].ch * arr[k][x3[2]].ch ;
				//printf("x1ch = %d*%d*%d\n", arr[i][x1[0]].ch, arr[j][x1[1]].ch, arr[k][x1[2]].ch);
				printf("%i %i %i| %4d/%4d, %4d/%4d, %4d/%4d\n", i, j, k, x1ch, x1zn, x2ch, x2zn, x3ch, x3zn);
			}

	return 0;
}
