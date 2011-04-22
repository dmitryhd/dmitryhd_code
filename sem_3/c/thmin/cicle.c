#include <stdio.h>
#include <math.h>
#define MAXN 10
#define INF 100000
int n, way[MAXN];
float min_len= INF;

typedef struct _point{
	int x, y;
}point;
point cicle[MAXN];

void set_way(int cur, int lev);
float get_dist(point a, point b);
void print_arr(int *a, int n);
float proc_len();

int main(int argc, char *argv[]){
	int x,y,i;
	scanf("%d\n",&n);	
	for(i=0;i<n;i++){
		scanf("%d %d\n",&x,&y);
		cicle[i].x=x;
		cicle[i].y=y;
	}
	set_way(0,1);
	printf("%f\n",min_len);
	return 0;
}
float get_dist(point a, point b){
	return sqrt(pow(a.x-b.x,2)+pow(a.y-b.y,2));
}
	
void set_way(int cur, int lev){
	int i,j, a[MAXN], cnt=0, flag=0;
	way[lev-1]=cur;
	//print_way();
	//get all unused points in a[]
	for(i=0;i<n;i++,flag=0){
		for(j=0;j<lev;j++){
			if(way[j]==i){
				flag=1;
				break;
			}
		}
		if(!flag)
			a[cnt++]=i;	
	}
	//in end 
	if(cnt==0){
		//printf("way:");
		//print_arr(way,n);
		proc_len();
		return;
	}
	for(i=0;i<cnt;i++)
		set_way(a[i],lev+1);
	
}
void print_arr(int *a, int n){
	int i;
	for(i=0;i<n;i++){
		printf("%d ",a[i]);
	}
	printf("\n");
}
float proc_len(){
	int i;
	float len=0;
	//sum length of the way
	for(i=1;i<=n;i++)
		len+=get_dist(cicle[way[i]],cicle[way[i-1]]);
	//plus last way
	len+=get_dist(cicle[way[n]],cicle[way[0]]);
	if(len<min_len)
		min_len=len;
	return len;
}
