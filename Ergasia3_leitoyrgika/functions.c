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



// void item_init(item *a,int type, int id ){
// 	a->type=type;
// 	a->id=id;
// 	a->constructed=true;
// 	a->sprayed=false;
// 	a->start=clock();		//the time starts to count for a item when it is created

// }

int val(char c) 
{ 
    if (c >= '0' && c <= '9') 
        return (int)c - '0'; 
    else if(c>='A' && c<='F')
        return (int)c - 'A' + 10; 
    else
    	return (int)c - 'a' + 10;
} 
  
// Function to convert a number from given base 'b' 
// to decimal 
int toDeci(char *str, int base) 
{ 
    int len = strlen(str); 
    int power = 1; // Initialize power of base 
    int num = 0;  // Initialize result 
    int i; 
  
    // Decimal equivalent is str[len-1]*1 + 
    // str[len-1]*base + str[len-1]*(base^2) + ... 
    for (i = len - 1; i >= 0; i--) 
    { 
        // A digit in input number must be 
        // less than number's base 
        if (val(str[i]) >= base) 
        { 
           printf("Invalid Number"); 
           return -1; 
        } 
  
        num += val(str[i]) * power; 
        power = power * base; 
    } 
  
    return num; 
} 
//push at the start of list
void pushP(part **Start,hashn data){		//list functions
	part *newp;
	newp=malloc(sizeof(part));
	newp->item1=malloc(sizeof(hashn));
	*(newp->item1)=data;
	newp->next=*Start;
	*Start=newp;
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

//returns the position of the element if it exists else returns -1
int search(part *Start,char *str1,char *str2){
	part *current=Start;
	int i=0;
	while(current !=NULL){
		if(strcmp(current->item1->p,str1)==0 && strcmp(current->item1->offs,str2)==0){
			//printf("EPISTREFEI TRUE\n");
			return i;
		}
		current=current->next;
		i++;
	}
	//printf("EPISTREFEI FALSE\n");
	return -1;
}

void ldelete(part **Start){		//delete a list

	part *current=*Start;
	part *next;
	while(current !=NULL){
		next=current->next;
		free(current);
		current=next;
	}
	*Start=NULL;
}

void lpr(part *Start){		//print a list
	part *current=Start;
	int i=0;
	while(current !=NULL){
		printf("%d %s%s/",i,current->item1->p,current->item1->offs);
		current=current->next;
		i++;
	}
	
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





void PM1(int num_p){
	char *shm;
	int i;

	
	
	int sem_id,shmid;

	sem_id = semget((key_t)keysem, 3,0666 );
	
	if (sem_id < 0) {
		perror("Could not open sem");
		exit(3);
	}

	shmid=shmget((key_t)keyshm1,10,0666);
	if(shmid<0){
		perror("shmget");
		exit(1);
	}


	shm=shmat(shmid,NULL,0);
	if(shm==(char*)(-1)){
		perror("shmat");
		exit(1);
	}

	FILE * fp;		//open file to read
	char * file="data1";
	char * line =NULL;
	size_t len =0;
	ssize_t read;
	fp = fopen(file,"r");
	if (fp==NULL){
		printf("MPLA\n");
		exit(EXIT_FAILURE);
	}
	for(i=0;i<num_p;i++){
		read=getline(&line,&len,fp);
		if (!semaphore_p(sem_id,0)) exit(EXIT_FAILURE);

		memcpy(shm,line,read);
		//printf("PRWTO\n");
		if (!semaphore_v(sem_id,2)) exit(EXIT_FAILURE);
		
	}
	
	
}



void PM2(int num_p){
	char *shm;
	int i;

	int sem_id,shmid;

	sem_id = semget((key_t)keysem, 3,0666 );
	
	if (sem_id < 0) {
		perror("Could not open sem");
		exit(3);
	}

	shmid=shmget((key_t)keyshm1,10,0666);
	if(shmid<0){
		perror("shmget");
		exit(1);
	}


	shm=shmat(shmid,NULL,0);
	if(shm==(char*)(-1)){
		perror("shmat");
		exit(1);
	}

	FILE * fp;
	char * file="data2";
	char * line =NULL;
	size_t len =0;
	ssize_t read;
	fp = fopen(file,"r");
	if (fp==NULL){
		printf("MPLA\n");
		exit(EXIT_FAILURE);
	}
	for(i=0;i<num_p;i++){
		read=getline(&line,&len,fp);
		if (!semaphore_p(sem_id,1)) exit(EXIT_FAILURE);

		memcpy(shm,line,read);
		//printf("DEYTERO\n");
		if (!semaphore_v(sem_id,2)) exit(EXIT_FAILURE);
		//printf("aaa %s\n",shm);
	}


}


void MM(int max_num, int q,int frame,int k ){
	char *shm;
	char *s;


	int i;
	int sem_id,shmid,shmid2;

	sem_id = semget((key_t)keysem, 3,0666 );
	if (sem_id < 0) {
		perror("Could not open sem");
		exit(3);
	}

	shmid=shmget((key_t)keyshm1, 10 ,0666);
	if(shmid<0){
		perror("shmget");
		exit(1);
	}

	shm=(char*)shmat(shmid,NULL,0);
	if(shm==(char*)-1){
		perror("shmat");
		exit(1);
	}

	int choice =0;		//gia thn enallagi
	int counter=0;

	int new_q=0;		//this happens if the q does not divine exactly the max_num

	int fr1count=0;
	int fr2count=0;

	int pag_fault1=0;
	int pag_fault2=0;

	int total_pag_fault=0;
	int total_W=0,total_R=0;

	int j,pos,found_pos;
	char *text=malloc(11);
	char *text1=malloc(6),*text2=malloc(4);
	part **Start1;
	part *total_s=NULL;

	Start1=malloc(frame*sizeof(part*));

	for(i=0;i<max_num;i++){
		
		if(max_num%q!=0 && max_num-i==max_num%(q*2) && new_q==0){	//new q base on the remaining pages
			new_q=(max_num%(q*2))/2;
			q=new_q;
		}
		if(fr1count+fr2count==frame ){
				fr2count=0;
				fr1count=0;
				pag_fault2=0;
				pag_fault1=0;
				for(j=0;j<frame;j++){
					ldelete(&Start1[j]);
				}
			}
		if (!semaphore_p(sem_id,2)) exit(EXIT_FAILURE);
		memcpy(text,shm,10);
		text[11]='\0';
		memcpy(text1,text,5);
		text1[6]='\0';
		memcpy(text2,text+5,3);
		text2[4]='\0';
		pos=toDeci(text1,16)%(frame/2);
		
		
		if(choice==0){					//PM1 turn
			if(pag_fault1==k+1){
				fr1count=0;
				pag_fault1=0;
				for(j=0;j<frame/2;j++){
					ldelete(&Start1[j]);
				}
			}
			fr1count++;
			if(found_pos=search(Start1[pos],text1,text2)==-1){
				pag_fault1++;
				total_pag_fault++;
				hashn page;
				memcpy(page.p,text1,5);
				memcpy(page.offs,text2,3);
				if(text[9]=='R'){
					page.rw='R';
					total_R++;
				}
				else if(text[9]=='W'){
					page.rw='W';
					total_W++;
				}
				
				page.dirt='0';
				pushP(&Start1[pos],page);
				pushP(&total_s,page);
				//printf("%s++++%s\n",Start1[pos]->item1->p,Start1[pos]->item1->offs );
			}
			else{
				part *current=Start1[pos];
				for(j=0;j<found_pos;j++){
					current=current->next;
				}
				pushP(&total_s,*(current->item1));
				if(text[9]=='W'&& current->item1->dirt==0){
					total_W++;
					current->item1->dirt=1;
				}
			}
		}
		else if(choice==1){			//PM2 turn
			if(pag_fault2==k+1){
				fr2count=0;
				pag_fault2=0;
				for(j=frame/2;j<frame;j++){
					ldelete(&Start1[j]);
				}
			}
			fr2count++;
			pos=pos+(frame/2);
			if(found_pos=search(Start1[pos],text1,text2)==-1){
				pag_fault2++;
				total_pag_fault++;
				hashn page;
				memcpy(page.p,text1,5);
				memcpy(page.offs,text2,3);
				if(text[9]=='R'){
					page.rw='R';
					total_R++;
				}
				else if(text[9]=='W'){
					page.rw='W';
					total_W++;
				}
				
				page.dirt='0';
				pushP(&Start1[pos],page);
				pushP(&total_s,page);
				//printf("%s++++%s\n",Start1[pos]->item1->p,Start1[pos]->item1->offs );
			}
			else{
				part *current=Start1[pos];
				for(j=0;j<found_pos;j++){
					current=current->next;
				}
				pushP(&total_s,*(current->item1));
				if(text[9]=='W' && current->item1->dirt==0){
					total_W++;
					current->item1->dirt=1;
				}
				
			}
		}
		counter++;
		if(counter==q){		//edw ginetai h enallagh
			counter=0;
			if(choice==0){
				choice=1;
				
			}
			else{
				choice=0;
				
			}
		}
		if(choice==0){
			//printf("ANOIGEI TO 1o\n");
			if (!semaphore_v(sem_id,0)) exit(EXIT_FAILURE);
		}
		else{
			//printf("ANOIGEI TO 2o\n");
			if (!semaphore_v(sem_id,1)) exit(EXIT_FAILURE);
		}

	}
	
	//lpr(total_s);
	printf("\n");
	printf("TOTAL WRITES TO DISK=%d\nTOTAL READS FROM DISK=%d\nTOTAL PAGE FAULT=%d\nCURRNET FRAMES USED=%d \n", total_W,total_R,total_pag_fault,fr1count+fr2count);
	
	//printf("%s++++%s\n",Start1[2]->item1->p,Start1[2]->item1->offs );

	
	

}
