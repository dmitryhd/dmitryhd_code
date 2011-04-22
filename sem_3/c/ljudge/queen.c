#include <stdio.h>
#include <stdlib.h>

#define DEBUG 0
#define OUTPUT 0

int main(){
	int N;
	scanf("%d",&N);
	int **a, i, j, ic, jc, *p, icc, j_up, j_down, counter, flag=1;
	a=(int **)malloc(N*sizeof(int *));
	for(i=0;i<N;i++)
		a[i]=(int *)malloc(N*sizeof(int));
	p=(int *)malloc(N*sizeof(int));
//��������� ������� ������.
	for(i=0;i<N;i++)
		for(j=0;j<N;j++)
			a[i][j]=0;
	i=0;
	counter=0;

//HERE WE GO :) �� ��� ��� ���� flag=1, ��������� ������� ���� � ���� ����������
	while(flag){
	//find zero. ���� � i�� ������ �� j� ����� 0 � ������ ���� �����.
		for(j=0;j<N;j++){
			if(a[i][j]==0){
			//ok, zero finded! �������, ����� ��. �����
				a[i][j]=2;
				p[i]=j;
			//delete all zero's ���� ��� ����, ������� �������� ����� ���������� ������� ����� ����������
				for(ic=0;ic<N;ic++)
					for(jc=0;jc<N;jc++)
						if(a[ic][jc]==1)
							a[ic][jc]=0;

				//MARK FIXED CELLS
				//������ �������� 1. ��� ����, ������� ������ ������� �������.							
				//----------------------------------------------------
				for(ic=i;ic<N;ic--){
					if(ic<0)
						break;
					jc=p[ic];
					if(a[ic][jc]==2){
						for(icc=ic+1;icc<N;icc++){
							j_up=jc+icc-ic;
							j_down=jc-icc+ic;
							if(a[icc][jc]!=2){
								a[icc][jc]=1;
							}
							if( (j_up<N) && (a[icc][j_up]!=2)){
								a[icc][j_up]=1;
							}
							if( (j_down>=0) && (a[icc][j_down]!=2)){
								a[icc][j_down]=1;
							}
		
						}	
					}
				}
				//print cobination. �������� ��, ��� ���������� ����� ����� ����������.
	
				//COMBINATION FINDED; �! �� ����� ��, ��� ������:
				if(i==N-1) {
					counter++;
					//print cobination.
					if(j==N-1){
						//������� ���� ������ � ������������� � ����������
						for(j=0;j<N;j++)
							//clear row i
							a[i][j]=0;
							//back to i--;
							i--;
						//delete all zero's
						for(ic=0;ic<N;ic++)
							for(jc=0;jc<N;jc++)
								if(a[ic][jc]==1)
									a[ic][jc]=0;
						//MARK FIXED CELLS
						//������ �������� 1. ��� ����, ������� ������ ������� �������.							
						for(ic=i;ic<N;ic--){
							if(ic<0)
								break;
							jc=p[ic];
							if(a[ic][jc]==2){
								for(icc=ic+1;icc<N;icc++){
									j_up=jc+icc-ic;
									j_down=jc-icc+ic;
									if(a[icc][jc]!=2){
										a[icc][jc]=1;
									}
									if( (j_up<N) && (a[icc][j_up]!=2)){
										a[icc][j_up]=1;
									}
									if( (j_down>=0) && (a[icc][j_down]!=2)){
										a[icc][j_down]=1;
									}
				
								}	
							}
						}
				//print cobination. �������� ��, ��� ���������� ����� ����� ����������.
					}
					break;		
				}			

				//next row (i) ��������� �� ����� ������
				i++;
				break;
			}	
			//don't find ;-( �� ����� �� ������.
			if(j==N-1){
				for(j=0;j<N;j++){
					//clear row i ������� ���� ������
					a[i][j]=0;
				}
				//back to i--; �������� �� ������ �����
				i--;
				//����� ��� ��� �����������?
				if(i<0){
					flag=0;
				}
				//�� ���. �������� ������.

	
				//MARK FIXED CELLS
				//������ �������� 1. ��� ����, ������� ������ ������� �������.							
				//----------------------------------------------------
				//������ ��� ����� ��������� ����� ������ ����� �������, �.�. ��� ��� ������ �� �����, 
				//��� ������ ���.
				for(ic=i-1;ic<N;ic--){
					if(ic<0)
						break;
					jc=p[ic];
					if(a[ic][jc]==2){
						for(icc=ic+1;icc<N;icc++){
							j_up=jc+icc-ic;
							j_down=jc-icc+ic;
							if(a[icc][jc]!=2){
								a[icc][jc]=1;
							}
							if( (j_up<N) && (a[icc][j_up]!=2)){
								a[icc][j_up]=1;
							}
							if( (j_down>=0) && (a[icc][j_down]!=2)){
								a[icc][j_down]=1;
							}
		
						}	
					}
				}
				//---------------------------------------------------

				break;
			}
		}
	}
//PRINT NUMBER OF COMBINATIONS
	printf("%d \n",counter);
	return 0;

}
