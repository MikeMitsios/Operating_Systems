#include <stdbool.h>
#include <time.h>

#define keyshm1 4321		//keys for shm
#define keyshm2 5432
#define keyshm3 6543
#define keysem 9871			//keys for semaphores

typedef struct Item{
	bool sprayed;
	bool constructed;
	int id;
	int type;
	clock_t start;
	clock_t end_a;
	clock_t end_b;
}item;

typedef struct PartNode{		//nodes for list
	item *item1;
	struct PartNode *next;
}part;


union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};




