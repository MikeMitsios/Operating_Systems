#include <stdlib.h>
#include <stdio.h>
#include <sys/sem.h>
#include "structs.h"
#include <unistd.h>
#include "funct.h"
#include <sys/shm.h>
#include <sys/types.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/wait.h>



int main(int argc,char *argv[]){


if(argc!=5){
	printf("LATHOS ORISMATA\n");	// definition of num_items->type x(1,2,3)
	exit(1);
}

int num_max=atoi(argv[1]);	
int q=atoi(argv[2]);
int frames=atoi(argv[3]);
int k=atoi(argv[4]);

pid_t pids[4];
int i;


int sem_id,shmid;

sem_id = semget((key_t)keysem, 3, IPC_CREAT | 0600);

if (sem_id < 0) {
perror("Could not create sem");					//semaphores creation 
exit(3);
}

if (!set_semvalue(sem_id,0)) {
		fprintf(stderr, "Failed to initialize semaphore 0\n");			//semaphores init with 0
		exit(EXIT_FAILURE);
	}

if (!set_semvalue(sem_id,1)) {
		fprintf(stderr, "Failed to initialize semaphore 1\n");
		exit(EXIT_FAILURE);
	}
if (!set_semvalue(sem_id,2)) {
		fprintf(stderr, "Failed to initialize semaphore 1\n");
		exit(EXIT_FAILURE);
	}
		


if (!semaphore_v(sem_id,0)) exit(EXIT_FAILURE);
		//this semaphore 0 are for write in shm and in order to pass the first p call needs to i have to up them



shmid=shmget((key_t)keyshm1, 10 ,IPC_CREAT | 0666);	//shm1 create
//printf("%d\n",shmid );
	if(shmid<0){
		perror("shmget");
		exit(1);
	}


for(i=0;i<4;i++){
	if((pids[i]=fork())<0){
		perror("fork");
		abort();
	}
	else if(pids[i]==0) {			//its proccess calls differen function
		switch(i){

			case 0:
				PM1(num_max/2);
				break;
			case 1:
				PM2(num_max/2);
				break;
			case 2:
				MM(num_max,q,frames,k);
				break;									
		}

		exit(0);

	}

}



int status;
pid_t pid;

while(i>0){				//parent waits to all proccess end
	pid=wait(&status);
	printf("Child with pid %ld \n",(long)pid);
	i--;
}

shmctl(shmid,IPC_RMID,NULL);		//free shared memory

del_semvalue(sem_id,0);			//delete semaphore


return 0;
}