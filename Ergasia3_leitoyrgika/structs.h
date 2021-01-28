#include <stdbool.h>
#include <time.h>

#define keyshm1 4321		//keys for shm
#define keysem 9871			//keys for semaphores






union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};


typedef struct Hashnode{
	char p[6];
	char offs[4];
	char rw;
	char dirt;
}hashn;

typedef struct PartNode{		//nodes for list
	hashn *item1;
	struct PartNode *next;
}part;