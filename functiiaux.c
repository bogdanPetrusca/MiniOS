//PETRUSCA BOGDAN - MIHAI
//313CB
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "structs.h"
#include "functiiaux.h"
#define MaxSize 3145728
//aceasta functie creeaza o celula de lista de memorie
TListaMem AlocaCelulaMem(int adrS, int adrE, int mem_size, int pid)
{
	TListaMem aux = (TListaMem) malloc(sizeof(TMemorie));
	aux->adr_start = adrS;
	aux->adr_end = adrE;
	aux->mem_size = aux->process_mem = mem_size;
	aux->pid = pid;
	aux->urm = NULL;
	return aux;
}
//functia aceasta aloca o stiva
void *InitS(size_t d)
{
	TStiva *s;
	s = (TStiva *)malloc(sizeof(TStiva));
	if(!s) return NULL;
	s->dime = d;
	s->vf = NULL;
	return (void *)s;
}
//functia aceasta aloca o coada
void *InitQ(size_t d)
{
	TCoada *c;
	c = (TCoada *)malloc(sizeof(TCoada));
	if(!c) return NULL;
	c->dime = d;
	c->ic = c->sc = NULL;
	return (void *)c;
}
//functia aceasta aloca un proces
TProcess *AlocaProces()
{
	TProcess *aux;
	aux = (TProcess *)malloc(sizeof(TProcess));
	aux->stack_size = aux->PID = aux->priority = aux->mem_size = aux->time = aux->exec_time = aux->adresa = aux->cuanta = 0;
	aux->stiva_proces = InitS(sizeof(int));
	return aux;
}
//functia aceasta creeaza o celula de lista generica in care va fi salvat un proces in cozi
TLG AlocaCelula(void *ax, size_t d)
{
	TLG aux;
	aux = (TLG) malloc(sizeof(TCelulaG));
	if(!aux) return NULL;
	aux->urm = NULL;
	aux->info = AlocaProces();
	if(!aux->info)
	{
		free(aux); 
		return NULL;
	}
	memcpy(aux->info, ax, d);
	return aux;
}
//functia aceasta adauga un element in stiva
int push(void *s, void *al)
{
	TLG aux = AlocaCelula(al, sizeof(TProcess));
	TStiva *s1 = (TStiva *)s;
	if(s1->vf == NULL)
		s1->vf = aux;
	else
	{
		aux->urm = s1->vf;
		s1->vf = aux;
	}
	return 1;
}
//functia aceasta extrage un element din stiva
int pop(void *s, void *al)
{
	TStiva *s1 = (TStiva *)s;
	if(s1->vf == NULL) return 0;
	memcpy(al, s1->vf->info, sizeof(int));
	TLG aux = s1->vf;
	s1->vf = aux->urm;
	free(aux->info);
	free(aux);
	return 1;
}
//functia aceasta adauga un element in coada
int IntrQ(void *c, void *al)
{
	TLG aux;
	aux = AlocaCelula(al, sizeof(TProcess));
	if(!aux) return 0;
	if(((TCoada *)c)->sc == NULL && ((TCoada *)c)-> ic == NULL)
		((TCoada *)c)->ic = ((TCoada *)c)->sc = aux;
	else
	{
		((TCoada *)c)->sc->urm = aux;
		((TCoada *)c)->sc = aux;
	}
	return 1;
}
//functia aceasta extrage un element din coada
int ExtrQ(void *c, void *al)
{
	TCoada *caux = (TCoada *)c;
	TLG aux = caux->ic;
	if(caux->ic == NULL)
	{
		caux->sc = NULL;
		return 0;
	}
	((TCoada *)c)->ic = ((TCoada *)c)->ic->urm;
	memcpy(al,aux->info, caux->dime);
	free(aux->info);
	free(aux);
	if(caux->ic == NULL)
		caux->sc = NULL;
	return 1;
}
//functia aceasta compara 2 procese pentru a vedea care dintre acestea trebuie
//adaugat primul ordonat in coada de asteptare
int compC(void *a, void *b)
{
	TProcess *p1 = (TProcess *)a;
	TProcess *p2 = (TProcess *)b;
	if(p1->priority > p2->priority)
		return 1;
	else
		if(p1->priority < p2->priority)
			return -1;
		else
			if(p1->priority == p2->priority)
			{
				if(p1->exec_time < p2->exec_time)
					return 1;
				else
					if(p1->exec_time > p2->exec_time)
						return -1;
					else
						if(p1->exec_time == p2->exec_time)
						{
							if(p1->PID < p2->PID)
								return 1;
							else
								return -1;
						}
			}
	return 0;
}
//cu ajutorul aceste functii adaug un proces ordonat intr-o coada
void add(void *c, void *al)
{
	TProcess *paux = AlocaProces();
	TCoada *caux = InitQ(sizeof(TProcess));
	//in cazul in care coada e goala, atunci procesul se adauga la inceput
	if(((TCoada *)c)->ic == NULL)
		IntrQ(c, al);
	else
	{
		//in caz contrar extrag un element din coada si il compar cu elementul
		//ce trebuie adaugat. Daca comparatia este mai mare ca 0 atunci adaugam
		//in coada prima data elementul al, iar dupa aceea elemntul extras. Daca
		//comparatia este mai mica ca 0 atunci adaugam doar elementul extras in coada.
		//daca totusi nu mai sunt elemente in coada, iar elementul de adaugat inca nu
		//a fost introdus, acesta se introduce in coada.
		while(ExtrQ(c, paux))
		{
			if(compC(al, paux) > 0)
			{
				IntrQ(caux, al);
				IntrQ(caux, paux);
				break;
			}
			else
			{
				IntrQ(caux, paux);
				if(((TCoada *)c)->ic == NULL)
					IntrQ(caux, al);
			}

		}
		//adaug in coada auxiliara elementele din c ramase neadaugate
		while(ExtrQ(c, paux))
			IntrQ(caux, paux);
		//adaug elementele inapoi in c 
		while(ExtrQ(caux, paux))
			IntrQ(c, paux);
	}
}

//functia aceasta cauta un proces in cele 3 cozi si ii determina starea
void get(void *c1, void *c2, void *c3, short int PID, FILE *f)
{
	TCoada *a = (TCoada *)c1;
	TCoada *b = (TCoada *)c2;
	TCoada *c = (TCoada *)c3;
	TCoada *caux = InitQ(sizeof(TProcess));
	TProcess *p = AlocaProces();
	int ok = 0;
	while(ExtrQ(a, p))
	{
		if(p->PID == PID)
		{
			ok = 1;
			fprintf(f, "Process %u is running (remaining_time: %u).\n",PID, p->exec_time );
		}
		IntrQ(caux, p);

	}
	while(ExtrQ(caux, p))
		IntrQ(a, p);

	while(ExtrQ(b, p))
	{
		if(p->PID == PID)
		{
			ok = 1;
			fprintf(f, "Process %u is waiting (remaining_time: %u).\n",PID, p->exec_time );
		}
		IntrQ(caux, p);
	}
	while(ExtrQ(caux, p))
		IntrQ(b, p);

	while(ExtrQ(c, p))
	{
		if(p->PID == PID)
		{
			ok = 1;
			fprintf(f, "Process %u is finished.\n",PID);
		}
		IntrQ(caux, p);
	}
	while(ExtrQ(caux, p))
		IntrQ(c, p);

	if(ok == 0)
		fprintf(f, "Process %u not found.\n", PID);	
}

//aceasta functia afiseaza coada de procese
//pentru a nu face 2 functii separate pentru cele 2 cozi am creat
//o variabila type pentru a vedea daca coada este cea de waiting
//sau cea de finished
void print(void *c, char *type, FILE *f)
{
	char Ctype[10], time_type[20];
	strcpy(Ctype, type);
	Ctype[0] -= 32;
	if(strcmp(type, "waiting") == 0)
		strcpy(time_type, "remaining_time");
	else
		strcpy(time_type, "executed_time");
	TCoada *caux = InitQ(sizeof(TProcess));
	TProcess *p = AlocaProces();
	fprintf(f, "%s queue:\n[",Ctype );
	while(ExtrQ(c, p))
	{
		if(strcmp(type, "waiting") == 0)
			fprintf(f, "(%u: priority = %d, %s = %u)",p->PID, p->priority, time_type, p->exec_time);
		else
			fprintf(f, "(%u: priority = %d, %s = %u)",p->PID, p->priority, time_type, p->time);			
		if(((TCoada *)c)->ic)
			fprintf(f, ",\n");
		IntrQ(caux, p);
	}
	fprintf(f, "]\n");
	while(ExtrQ(caux, p))
	{
		IntrQ(c, p);
	}
}

//aceasta functie adauga un numar in stiva procesului
//intai cauta procesul in cele coada de waiting si cea de
//running. Daca a gasit procesul face push la numarul de adaugat
//in stiva si se scad 4 bytes din memoria procesului
void pushC(short int pid, int data, void *a, void *b, FILE *f)
{
	TCoada *c1 = (TCoada *)a;
	TCoada *c2 = (TCoada *)b;
	TCoada *caux = InitQ(sizeof(TProcess));
	TProcess *p = AlocaProces();
	int ok = 0;
	while(ExtrQ(c1, p))
	{
		if(p->PID == pid)
		{
			if(p->stack_size < 4)
			{
				fprintf(f, "Stack overflow PID %u.\n", p->PID);
			}
			else
			{
				push(p->stiva_proces, &data);
				p->stack_size -= 4;
			}
			ok = 1;
		}
		IntrQ(caux, p);
	}
	while(ExtrQ(caux, p))
		IntrQ(c1, p);

	if(ok == 1)
		return;

	while(ExtrQ(c2, p))
	{
		if(p->PID == pid)
		{
			if(p->stack_size < 4)
			{
				fprintf(f, "Stack overflow PID %u.\n", p->PID);
			}
			else
			{
				push(p->stiva_proces, &data);
				p->stack_size -= 4;

			}
			ok = 1;
		}
		IntrQ(caux, p);
	}
	while(ExtrQ(caux, p))
		IntrQ(c2, p);

	if(ok == 0)
	{
		fprintf(f, "PID %u not found.\n", pid);
	}
}

//aceasta functia afiseaza stiva fiecarui proces. Functia cauta
//procesul in coada de waiting si cea de running, in caz ca a gasit
//procesul cu pidul data, afiseaza stiva lui.
void print_stack(void *a, void *b, short int pid, FILE *f)
{
	TCoada *c1 = (TCoada *)a;
	TCoada *c2 = (TCoada *)b;
	TCoada *caux = InitQ(sizeof(TProcess));
	TCoada *saux = InitS(sizeof(TProcess));
	TProcess *p = AlocaProces();
	int ok = 0;
	while(ExtrQ(c1, p))
	{
		if(p->PID == pid)
		{
			int data;
			ok = 1;
			if(p->stiva_proces->vf == NULL)
			{
				fprintf(f, "Empty stack PID %d.\n", p->PID);
			}
			else
			{
				fprintf(f, "Stack of PID %d:",p->PID);
				while(pop(p->stiva_proces, &data))
					push(saux, &data);
				while(pop(saux, &data))
				{
					fprintf(f, " %d", data);
					push(p->stiva_proces, &data);
				}
				fprintf(f, ".\n");
			}
		}
		IntrQ(caux, p);
	}
	while(ExtrQ(caux, p))
		IntrQ(c1, p);
	
	while(ExtrQ(c2, p))
	{
		if(p->PID == pid)
		{
			int data;
			ok = 1;
			if(p->stiva_proces->vf == NULL)
			{
				fprintf(f, "Empty stack PID %d.\n", p->PID);
			}
			else
			{
				fprintf(f, "Stack of PID %d:",p->PID);
				while(pop(p->stiva_proces, &data))
					push(saux, &data);
				while(pop(saux, &data))
				{
					fprintf(f, " %d", data);
					push(p->stiva_proces, &data);
				}
				fprintf(f, ".\n");
			}
		}
		IntrQ(caux, p);
	}
	while(ExtrQ(caux, p))
		IntrQ(c2, p);


	if(ok == 0)
		fprintf(f, "PID %d not found.\n", pid );
		
}

//aceasta functie extrage din stiva procusului cu pidul dat primul element
void popC(void *a, void *b, int pid, FILE *f)
{
	TCoada *c_waiting = (TCoada *)a;
	TCoada *c_running = (TCoada *)b;
	TCoada *caux = InitQ(sizeof(TProcess));
	TProcess *p = AlocaProces();
	int data;
	int ok = 0;
	while(ExtrQ(c_waiting, p))
	{
		if(p->PID == pid)
		{
			if(p->stiva_proces->vf == NULL)
				fprintf(f, "Empty stack PID %d.\n",pid);
			else
			{
				pop(p->stiva_proces, &data);
				p->stack_size += 4;
			}
			ok = 1;
		}
		IntrQ(caux, p);
	}

	while(ExtrQ(caux, p))
		IntrQ(c_waiting, p);

	if(ok == 1)
		return;

	while(ExtrQ(c_running, p))
	{
		if(p->PID == pid)
		{
			if(p->stiva_proces->vf == NULL)
				fprintf(f, "Empty stack PID %d.\n",pid);
			else
			{
				pop(p->stiva_proces, &data);
				p->stack_size += 4;
			}
			ok = 1;
		}
		IntrQ(caux, p);
	}

	while(ExtrQ(caux, p))
		IntrQ(c_running, p);

	if(ok == 0)
		fprintf(f, "PID %d not found.\n",pid);
}

int min(int a, int b, int c)
{
	int Min = a;
	if(b < Min)
		Min = b;
	if(c < Min)
		Min = c;
	return Min;
}
//functia lucreaza astfel: se face minimul dintre run_time, cuanta, si timpul de executie
//ramas al procesului si scad din cuanta fiecarui proces si din timpul acestuia acest minim.
//
void run(int run_time, int cuanta, void *a, void *b, void *c, PID *v, TListaMem l)
{
	TCoada *c_waiting = (TCoada *)a;
	TCoada *c_running = (TCoada *)b;
	TCoada *c_finished = (TCoada *)c;
	TProcess *p = AlocaProces(), *q = AlocaProces();
	while(run_time > 0)
	{
		//se extrage procesul din running
		ExtrQ(c_running, p);
		
		int var = min(p->exec_time, p->cuanta, run_time);
		p->cuanta -= var;
		p->exec_time -= var;

		//in cazul in care timpul de executie al procesului este 0 i se elibereaza valabilitatea
		//pidului. Lista l este cea in care retin memoria sistemului. Voi vorbi putin mai jos despre
		//ea. In cazul in care procesul s-a terminat atribui campului mem_size din lista valoarea -1.

		if(p->exec_time <= 0)
		{
			v[p->PID - 1].valabilitate = 1;
			TListaMem aux = l;
			for(; aux != NULL; aux = aux->urm)
			{
				if(aux->pid == p->PID)
				{
					aux->mem_size = -1;
				}
			}
			//introduc in coada finished procesul si extrag din coada de waiting urmatorul proces,
			//daca acesta exista, pe care il introduc imediat in runnig
			if(p->PID != 0)
				IntrQ(c_finished, p);
			if(!ExtrQ(c_waiting, p)) {
				break;}
		
				IntrQ(c_running, p);
			
		}
		//in cazul in cuata procesului este mai mica ca 0, adica daca procesul nu s-a terminat,
		//insa i s-a terminat cuanta, extrag noul proces din waiting, iar pe procesul cu cuanta terminata
		//il adaug in coada de waiting. In cazul in care un nu se mai afla niciun proces in coada de waiting
		//adaug iar procesul cu cuanta terminata in running
		else if(p->cuanta <= 0)
		{
			p->cuanta = cuanta;
			if(ExtrQ(c_waiting, q))
			{
			

				add(c_waiting, p);
				IntrQ(c_running, q);
			}
			else
				IntrQ(c_running, p);
		}
		//in cazul in care procesul nu i s-a terminat nici cuanta nici exec_time-ul este introdus inapoi in running
		else
		{
			IntrQ(c_running, p);
		}
		//scad din run_time pe var
		run_time -= var;
	}
}	

//functia afiseaza timpul ramas
int finish(void *a, void *b)
{
	TCoada *c1 = (TCoada *)a;
	TCoada *c2 = (TCoada *)b;
	TCoada *caux = InitQ(sizeof(TProcess));
	TProcess *p = AlocaProces();
	int s = 0;
	while(ExtrQ(c1, p))
	{
		s += p->exec_time;
		IntrQ(caux, p);
	}
	while(ExtrQ(caux, p))
		IntrQ(c1, p);

	while(ExtrQ(c2, p))
	{
		s += p->exec_time;
		IntrQ(caux, p);
	}
	while(ExtrQ(caux, p))
		IntrQ(c2, p);
	return s;
}
//functia adauga un proces in memorie. In fiecare celula este retinuta adresa de inceput, adresa de sfarsit
//memoria si pidul fiecarui proces. In cazul in care un proces se termina mem_size-ul acestuia din lista de memorie
//se va face -1
int AddInMem(TListaMem *aL, int mem_size, int pid)
{
	int ok = 0, var = 0, ok1 = 0;
	TListaMem ant, u = NULL, p = *aL, q = *aL;
	if(p != NULL)
    {
        while(p->urm)
            p = p->urm;
        u = p;
    }
    //daca lista este goala se adauga la inceput procesul cu adresa de inceput 0 si adresa de final mem_size
    if(u == NULL)
    { 
    	var = 0;
    	TListaMem aux = AlocaCelulaMem(0, mem_size, mem_size, pid);
        *aL = aux;
    }
    else
    {
    	for(ant = NULL; q != NULL; ant = q, q = q->urm)
    	{
    		//in caz contrar se parcurge lista. Daca se gaseste o celula cu mem_size -1 adica un spatiu liber
    		//se verifica daca procesul ce trebuie adaugat in lista poate fi adaugat in celula respectiva
    		//facand difrenta dintre adresa de inceput si cea de sfarsit a spatiului gol. In cazul in care
    		//diferenta este egala cu marimea procesului atunci acesta se adauga pe tot spatiul.
    		if(q->mem_size == -1)
    		{
    			if(q->adr_end - q->adr_start == mem_size)
    			{
    				if(q == *aL)
    				{
    					TListaMem z = *aL;
    					var = 0;
    					TListaMem aux1 = AlocaCelulaMem(0, mem_size, mem_size, pid);
    					*aL = aux1;
    					aux1->urm = z->urm;
    				}
    				else
    				{
    					var = q->adr_start;
	    				TListaMem aux = AlocaCelulaMem(q->adr_start, q->adr_end, mem_size, pid);
	    				ant->urm = aux;
	    				aux->urm = q->urm;
					}
					ok1 = 1;
    			}
    			//in cazul in care spatiul gol este mai mare decat marimea procesului atunci se creeaza
    			//2 celule. In prima se va retine memoria procesului, iar a 2a va fi un spatiu gol de marime ramasa
    			else if(q->adr_end - q->adr_start > mem_size)
    			{
    				if(q == *aL)
    				{
    					var = 0;
    					TListaMem aux1 = AlocaCelulaMem(0, mem_size, mem_size, pid);
    					TListaMem aux2 = AlocaCelulaMem(mem_size, q->adr_end, -1, -1);
    					TListaMem z = *aL;
    					*aL = aux1;
    					aux1->urm = aux2;
    					aux2->urm = z->urm;

    				}
    				else
    				{
    					var = q->adr_start;
	    				TListaMem aux1 = AlocaCelulaMem(q->adr_start, q->adr_start + mem_size, mem_size, pid);
	    				TListaMem aux2 = AlocaCelulaMem(q->adr_start + mem_size, q->adr_end, -1, -1);
	    				ant->urm = aux1;
	    				aux1->urm = aux2;
	    				aux2->urm = q->urm;
					}
					ok1 = 1;
    			}
    			//in cazul in care procesul nu incape in spatiile goale, iar ultima celula este un spatiul gol, procesul se adauga peste aceasta
    			else if(q->urm == NULL)
    			{
    				var = q->adr_start;
    				TListaMem aux = AlocaCelulaMem(var, var + mem_size, mem_size, pid);
    				q->urm = aux;
    				ok1 = 1;
    			}
    			else
    			{
    				continue;
    			}
    			break;
    		}
    		else
    		{
    			ok = 1;
    		}
    	}
    	//in cazul in care procesul nu incape in spatiile goale sau nu exista astfel de spatii atunci se adauga la sfarsit
    	if(ok == 1 && ok1 != 1)
    	{
    		if(u->adr_end + mem_size > MaxSize)
    			return -1;
    		var = u->adr_end;
    		TListaMem aux = AlocaCelulaMem(u->adr_end, u->adr_end + mem_size, mem_size, pid);
    		u->urm = aux;
    	}

    }
    return var;
}

//aceasta functie elimina spatiile goale din lista de memorie in cazul in care nu mai este loc
//pentru a adauga un alt proces
void defragmentare(AlistaMem aL)
{
	TListaMem p = *aL, aux, q, ant;
	ant = NULL;
	while(p)
	{
		//in cazul in care se gaseste un spatiul gol aceasta este eliminat, iar peste urmatoarele celule
		//se actualizeaza adresele
		if(p->mem_size == -1)
		{
			if(ant == NULL)
				*aL = p->urm;
			else
				ant->urm = p->urm;
			aux = p;
			p = p->urm;
			q = p;
			for(; q != NULL; q = q->urm)
			{
				q->adr_start -= aux->process_mem;
				q->adr_end -= aux->process_mem;
			}
			free(aux);
		}
		else
		{
			ant = p;
			p = p->urm;
		}
	}
}

//aceasta functiie combina 2 sau mai multe spatii goale intr-unul
void ElimDuble(TListaMem l)
{
	for(; l != NULL && l->urm != NULL; l = l->urm)
	{
		//in cazul in care atat celula curenta cat si cea urmatoare au mem_size = -1 atunci se parcurge
		//de la celula urmatoare pana cand se gaseste un spatiu ocupat. Se actualizeaza adresa de sfarsit
		//a ceululei curente goale si se fac noile legaturi.
		TListaMem q, z;
		if(l->mem_size == -1 && l->urm->mem_size == -1)
		{
			z = l;
			TListaMem p = l->urm;
			while(p->mem_size == -1 && p)
			{
				q = p;
				if(!p->urm)
					break;
				else
					p = p->urm;
			}
			z->adr_end = q->adr_end;
			z->urm = q->urm;

		}
		else
			continue;
	}
}
