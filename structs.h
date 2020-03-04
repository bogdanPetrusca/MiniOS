//PETRUSCA BOGDAN - MIHAI
//313CB
#ifndef structs
#define structs

typedef struct celulaG
{
	void *info;
	struct celulaG *urm;
}TCelulaG, *TLG, **ALG;

typedef struct
{
	size_t dime;
	TLG vf;
}TStiva;

typedef struct
{
	size_t dime;
	TLG ic, sc;
}TCoada;

typedef struct
{
	short int PID;
	int priority;
	char stare[10];
	unsigned int exec_time, mem_size, stack_size;
	int adresa;
	int cuanta;
	int time;
	TStiva *stiva_proces;
}TProcess;

typedef struct celula
{
	int adr_start, adr_end, mem_size, pid, process_mem;
	struct celula *urm;
}TMemorie, *TListaMem, **AlistaMem;

typedef struct
{
	int pid;
	int valabilitate;
}PID;
#endif