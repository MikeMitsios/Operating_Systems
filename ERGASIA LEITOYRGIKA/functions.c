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
#include <time.h>



void item_init(item *a,int type, int id ){
	a->type=type;
	a->id=id;
	a->constructed=true;
	a->sprayed=false;
	a->start=clock();		//the time starts to count for a item when it is created

}



void pushP(part **Start,item data){		//list functions
	part *newp;
	newp=malloc(sizeof(part));
	newp->item1=malloc(sizeof(item));
	*(newp->item1)=data;
	newp->next=*Start;
	*Start=newp;
}

item* Startp(part *Start){
	return Start->item1;
}

void popP(part **Start){
	part *temp=NULL;
	temp=(*Start)->next;
	free(*Start);
	*Start=temp;
}

int is_emptyp(part *Start){			
	return Start==NULL;
}



double time_cal(clock_t a,clock_t b){		//time calculator
	return ((double)(b-a));	// /CLOCKS_PER_SEC
}




int set_semvalue(int sem_id, int sem_idx)	//functions for semaphores
{
	union semun sem_union;
	sem_union.val = 0;
	if (semctl(sem_id, sem_idx, SETVAL, sem_union) == -1) return(0);
		return(1);
}


void del_semvalue(int sem_id,int sem_idx)
{
	union semun sem_union;
	if (semctl(sem_id, sem_idx, IPC_RMID, sem_union) == -1)
		fprintf(stderr, "Failed to delete semaphore\n");
}


int semaphore_p(int sem_id,int sem_idx)
{
	struct sembuf sem_b;
	sem_b.sem_num = sem_idx;
	sem_b.sem_op = -1; 
	sem_b.sem_flg = 0;
	//printf("EINAI PRIN TO SEMOP p %d\n",sem_idx);
	if (semop(sem_id, &sem_b, 1) == -1) {
		perror("ERROR P");
		fprintf(stderr, "semaphore_p failed\n");
		return(0);
	}
	return(1);
}

int semaphore_v(int sem_id,int sem_idx)
{
	struct sembuf sem_b;
	sem_b.sem_num = sem_idx;
	sem_b.sem_op = 1; 
	sem_b.sem_flg = 0;
	//printf("EINAI PRIN TO SEMOP v %d\n",sem_idx);
	if (semop(sem_id, &sem_b, 1) == -1) {
		perror("ERROR V");
		fprintf(stderr, "semaphore_v failed\n");
	return(0);
	}
	return(1);
}





void construction(int num_p,int type){
	item *shm;
	//part* pnt;
	int i;

	srand(getpid());		//to get different rand for id i mode srand with getpid()
	
	int sem_id,shmid;

	sem_id = semget((key_t)keysem, 8,0666 );
	
	if (sem_id < 0) {
		perror("Could not open sem");
		exit(3);
	}

	shmid=shmget((key_t)keyshm1, sizeof(item) ,0666);
	if(shmid<0){
		perror("shmget");
		exit(1);
	}


	shm=(item*)shmat(shmid,NULL,0);
	if(shm==(item*)-1){
		perror("shmat");
		exit(1);
	}

	for(i=0;i<num_p;i++){
		item item1;
		item_init(&item1,type,(rand()%9000)+1000);			//item is created with an id of 4 digits

		if (!semaphore_p(sem_id,0)) exit(EXIT_FAILURE);		//construction writes to shm1

		memcpy(shm,&item1,sizeof(item));
		//printf("DHMIOYRGHTHIKE TO %d\n",item1.id );			

		if (!semaphore_v(sem_id,1)) exit(EXIT_FAILURE);		//construction lets paint read from shm1
	}

	
	
}

void painting(int num_p){
	item *shm;
	item *shm2;
	char *s;


	int i;
	int sem_id,shmid,shmid2;

	sem_id = semget((key_t)keysem, 8,0666 );
	if (sem_id < 0) {
		perror("Could not open sem");
		exit(3);
	}

	shmid=shmget((key_t)keyshm1, sizeof(item) ,0666);
	if(shmid<0){
		perror("shmget");
		exit(1);
	}

	shm=(item*)shmat(shmid,NULL,0);
	if(shm==(item*)-1){
		perror("shmat");
		exit(1);
	}

	shmid2=shmget((key_t)keyshm2, sizeof(item) ,0666);
	if(shmid2<0){
		perror("shmget");
		exit(1);
	}

	shm2=(item*)shmat(shmid2,NULL,0);
	if(shm2==(item*)-1){
		perror("shmat");
		exit(1);
	}


	for(i=0;i<num_p*3;i++){


		if (!semaphore_p(sem_id,1)) exit(EXIT_FAILURE);		//wait until one construction writes on shared memory1
		shm->end_a=clock();

		//printf("BAFTIKE TO %d\n",shm->id );
		shm->sprayed=true;					//is painted

		sleep(shm->type);		
	

		if (!semaphore_p(sem_id,5)) exit(EXIT_FAILURE);		//writes to shm2

		memcpy(shm2,shm,sizeof(item));

		if (!semaphore_v(sem_id,0)) exit(EXIT_FAILURE);			//let the next construction on turn to write on shm1



		if(shm2->type==1){
			if (!semaphore_v(sem_id,2)) exit(EXIT_FAILURE); 	//let the check of type 1 to start
		}
		else if(shm2->type==2){
			if (!semaphore_v(sem_id,3)) exit(EXIT_FAILURE);		//let the check of type 2 to start
		}
		else{
			if (!semaphore_v(sem_id,4)) exit(EXIT_FAILURE);		//let the check of type 3 to start
		}


	}

}


void check(int type,int num_p){

	item *shm2;
	item *shm3;	
	int i,shmid2,shmid3;


	shmid2=shmget((key_t)keyshm2, sizeof(item) ,0666);
		if(shmid2<0){
			perror("shmget");
			exit(1);
		}

	shm2=(item*)shmat(shmid2,NULL,0);
		if(shm2==(item*)-1){
			perror("shmat");
			exit(1);
		}

	shmid3=shmget((key_t)keyshm3, sizeof(item) ,0666);
		if(shmid3<0){
			perror("shmget");
			exit(1);
		}

	shm3=(item*)shmat(shmid3,NULL,0);
		if(shm3==(item*)-1){
			perror("shmat");
			exit(1);
		}	


	int sem_id = semget((key_t)keysem, 8,0666 );
	if (sem_id < 0) {
		perror("Could not open sem");
		exit(3);
	}	



	for(i=0;i<num_p;i++){

		item item1;

		if(type==1){
			if (!semaphore_p(sem_id,2)) exit(EXIT_FAILURE);		//read from shm2
			item1=*shm2;
			sleep(1);
			if(item1.sprayed==true){
				//printf("TO ANTIKEIMENO %d EINAI BAMENO______________________\n",item1.id );
			}
			if (!semaphore_v(sem_id,5)) exit(EXIT_FAILURE);		// let the paint to rewrite
		}
		else if(type==2){
			if (!semaphore_p(sem_id,3)) exit(EXIT_FAILURE);		//read from shm2
			item1=*shm2;
			sleep(2);
			if(item1.sprayed==true){
				//printf("TO ANTIKEIMENO %d EINAI BAMENO______________________\n",item1.id );
			}
			if (!semaphore_v(sem_id,5)) exit(EXIT_FAILURE);		// let the paint to rewrite
		}
		else if(type==3){
			if (!semaphore_p(sem_id,4)) exit(EXIT_FAILURE);		//read from shm2
			item1=*shm2;
			sleep(3);
			if(item1.sprayed==true){
				//printf("TO ANTIKEIMENO %d EINAI BAMENO______________________\n",item1.id );
			}
			if (!semaphore_v(sem_id,5)) exit(EXIT_FAILURE);			// let the paint to rewrite
		}



		if (!semaphore_p(sem_id,6)) exit(EXIT_FAILURE);			//write to shm3

		memcpy(shm3,&item1,sizeof(item));

		if (!semaphore_v(sem_id,7)) exit(EXIT_FAILURE);			//let the structure to read

	}

}





void structure(int num_p){

	int shmid3;
	item* shm3;

	shmid3=shmget((key_t)keyshm3, sizeof(item) ,0666);
		if(shmid3<0){
			perror("shmget");
			exit(1);
		}

	shm3=(item*)shmat(shmid3,NULL,0);
		if(shm3==(item*)-1){
			perror("shmat");
			exit(1);
		}

	int sem_id = semget((key_t)keysem, 7,0666 );

	if (sem_id < 0) {
		perror("Could not open sem");
		exit(3);
	}	

	int i;

	part *Start1=NULL;
	part *Start2=NULL;
	part *Start3=NULL;

	double totalt_p=0,total_t=0;
	int id1,id2,id3,temp;
	char str1[12],str2[12],str3[12],strid[39];


	for(i=0;i<num_p*3;i++){
		item item1;

		if (!semaphore_p(sem_id,7)) exit(EXIT_FAILURE);
		item1=*shm3;
		if (item1.type==1){							//go to the right list depends on type
			pushP(&Start1,item1);
			//printf("PERNANE OLA 1111111111111111 %d\n",Startp(Start1)->id);
		}
		else if(item1.type==2){
			pushP(&Start2,item1);
			//printf("PERNANE OLA 222222222222222 %d\n",Startp(Start2)->id);

		}
		else if(item1.type==3){
			pushP(&Start3,item1);
			//printf("PERNANE OLA 333333333333333 %d\n",Startp(Start3)->id);

		}

		if(!(is_emptyp(Start1) || is_emptyp(Start2) || is_emptyp(Start3))){
			
			id1=Startp(Start1)->id;
			sprintf(str1,"%d",id1);

			sleep(1);
			
			totalt_p=totalt_p+time_cal(Startp(Start1)->start,Startp(Start1)->end_a);		//calculate the sum of paint latance
			Startp(Start1)->end_b=clock();
			total_t=total_t+time_cal(Startp(Start1)->start,Startp(Start1)->end_b);			//calculate the total sum of all
			popP(&Start1);

			id2=Startp(Start2)->id;
			sprintf(str2,"%d",id2);


			totalt_p=totalt_p+time_cal(Startp(Start2)->start,Startp(Start2)->end_a);
			Startp(Start2)->end_b=clock();
			total_t=total_t+time_cal(Startp(Start2)->start,Startp(Start2)->end_b);
			popP(&Start2);

			id3=Startp(Start3)->id;
			sprintf(str3,"%d",id3);
			

			totalt_p=totalt_p+time_cal(Startp(Start3)->start,Startp(Start3)->end_a);
			Startp(Start3)->end_b=clock();
			total_t=total_t+time_cal(Startp(Start3)->start,Startp(Start3)->end_b);
			popP(&Start3);


			strcpy(strid,str1);
			strcat(strid,str2);
			strcat(strid,str3);					//final id

			
			printf("ID TELIKOY PRODUCT %s \n",strid);	//print final id



		}
		else{
			//printf("DEN YPARXOYN KAI TA 3 \n");
		}

		if (!semaphore_v(sem_id,6)) exit(EXIT_FAILURE);
	}	

	printf("MESOS XRONOS ANAMONHS STO BAFEIO %f\n",totalt_p/(num_p*3) );		//print the avg
	printf("MESOS XRONOS GIA OLOKLHRVSH %f\n",total_t/num_p );					//print the avg

}
