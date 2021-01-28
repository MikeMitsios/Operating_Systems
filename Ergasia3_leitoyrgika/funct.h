void PM1(int );
void PM2(int );
void MM	(int ,int,int ,int);


void pushP(part **,hashn ); //funct for list
int val(char);
int toDeci(char *, int );
int search(part *,char *,char *);
void lpr(part *);
void ldelete(part**);
void popP(part **);
int is_emptyp(part *);


int set_semvalue(int,int);
void del_semvalue(int, int);
int semaphore_p(int ,int );
int semaphore_v(int ,int );