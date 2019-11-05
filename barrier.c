#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

#define N	4


struct barrier{
	int n;
	int count;
	pthread_mutex_t lock;
	pthread_cond_t cond;

}b;


void barrier_init(struct barrier *b,int n){
	b->n=n;	
	b->count = 0;
	pthread_mutex_init(&b->lock,NULL);
	pthread_cond_init(&b->cond,NULL);
}

void barrier_wait(struct barrier *b){
	int state=0;	
	pthread_mutex_lock(&b->lock);
	b->count++;
	if (b->count==N){
		state = 1;
		pthread_cond_broadcast(&b->cond);
		b->count=0;
	}	
	while (b->count < b->n && state!=1){
		pthread_cond_wait(&b->cond,&b->lock);
		state=1;
	}
	pthread_mutex_unlock(&b->lock);
	
}

void barrier_destroy(struct barrier *b){
	if (b->count==0){
		b=NULL;
	}
	else puts("barier in use"); 
}

void *t_func(void * arg){
	int j;
	int i;
	int k=0;
	for (i=0; i<100000; i++){		
		for (j=0; j<10000; j++){		
			k++;	
		}
		barrier_wait(&b);
	}
}

int main(){	

	struct timeval start,end;
	gettimeofday(&start,NULL);
	barrier_init(&b,N);
	pthread_t id[N];
	for (int i=0; i<N; i++)
		pthread_create(&id[i],NULL,t_func,NULL);
	for (int i=0; i<N; i++)
		pthread_join(id[i],NULL);	
	barrier_destroy(&b);
	gettimeofday(&end,NULL);
	int total_time=(end.tv_sec-start.tv_sec)*1000;
	total_time+=(end.tv_usec-start.tv_usec)/1000;
	printf ("total time in msecs: %d\n",total_time);	
	return 0;
}

