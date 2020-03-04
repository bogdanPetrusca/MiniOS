//PETRUSCA BOGDAN - MIHAI
//313CB
TListaMem AlocaCelulaMem(int adrS, int adrE, int mem_size, int pid);

void *InitS(size_t d);

void *InitQ(size_t d);

TLG AlocaCelula(void *ax, size_t d);

int push(void *s, void *al);

int pop(void *s, void *al);

int IntrQ(void *c, void *al);

int ExtrQ(void *c, void *al);

TProcess *AlocaProces();

int compC(void *a, void *b);

void add(void *c, void *al);

void get(void *c1, void *c2, void *c3, short int PID, FILE *f);

void print(void *c, char *type, FILE *f);

void pushC(short int pid, int data, void *a, void *b, FILE *f);

void print_stack(void *a, void *b, short int pid, FILE *f);

void run(int time, int max_running_time, void *a, void *b, void *c, PID *v, TListaMem l);

int min(int a, int b, int c);

void popC(void *a, void *b, int pid, FILE *f);

int finish(void *a, void *b);

int AddInMem(TListaMem *l, int mem_size, int pid);

void defragmentare(AlistaMem aL);

void ElimDuble(TListaMem l);