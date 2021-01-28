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


if(argc!=2){
	printf("LATHOS ORISMATA\n");	// definition of num_items->type x(1,2,3)
	exit(1);
}

int num_items=atoi(argv[1]);	

pid_t pids[8];
int i;


int sem_id,shmid,shmid2,shmid3;

sem_id = semget((key_t)keysem, 8, IPC_CREAT | 0600);

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
if (!set_semvalue(sem_id,3)) {
		fprintf(stderr, "Failed to initialize semaphore 1\n");
		exit(EXIT_FAILURE);
	}
if (!set_semvalue(sem_id,4)) {
		fprintf(stderr, "Failed to initialize semaphore 1\n");
		exit(EXIT_FAILURE);
	}	
if (!set_semvalue(sem_id,5)) {
		fprintf(stderr, "Failed to initialize semaphore 1\n");
		exit(EXIT_FAILURE);
	}
if (!set_semvalue(sem_id,6)) {
		fprintf(stderr, "Failed to initialize semaphore 1\n");
		exit(EXIT_FAILURE);
	}
if (!set_semvalue(sem_id,7)) {
		fprintf(stderr, "Failed to initialize semaphore 1\n");
		exit(EXIT_FAILURE);
	}		


if (!semaphore_v(sem_id,0)) exit(EXIT_FAILURE);
if (!semaphore_v(sem_id,5)) exit(EXIT_FAILURE);				//these semaphores 0-5-6 are for write in shm and in order to pass the first p call needs to i have to up them
if (!semaphore_v(sem_id,6)) exit(EXIT_FAILURE);


//printf("AAAAAAAAAAAAAAAAAAAAA%d\n",sem_id );


shmid=shmget((key_t)keyshm1, sizeof(item) ,IPC_CREAT | 0666);	//shm1 create
//printf("%d\n",shmid );
	if(shmid2<0){
		perror("shmget");
		exit(1);
	}

shmid2=shmget((key_t)keyshm2, sizeof(item) ,IPC_CREAT | 0666);	//shm2 create
//printf("%d\n",shmid2 );
	if(shmid<0){
		perror("shmget");
		exit(1);
	}

shmid3=shmget((key_t)keyshm3, sizeof(item) ,IPC_CREAT | 0666);	//shm3 create
//printf("%d\n",shmid3 );
	if(shmid<0){
		perror("shmget");
		exit(1);
	}

for(i=0;i<8;i++){
	if((pids[i]=fork())<0){
		perror("fork");
		abort();
	}
	else if(pids[i]==0) {			//its proccess calls differen function
		switch(i){

			case 0:
				construction(num_items,1);
				break;
			case 1:
				construction(num_items,2);
				break;
			case 2:
				construction(num_items,3);
				break;
			case 3:
				painting(num_items);
				break;
			case 4:
				check(1,num_items);
				break;
			case 5:
				check(2,num_items);
				break;	
			case 6:
				check(3,num_items);
				break;
			case 7:
				structure(num_items);
				break;										
		}

		exit(0);

	}

}



int status;
pid_t pid;

while(i>0){				//parent waits to all proccess end
	pid=wait(&status);
	//printf("Child with pid %ld \n",(long)pid);
	i--;
}

return 0;
}