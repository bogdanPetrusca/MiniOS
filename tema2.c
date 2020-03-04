//PETRUSCA BOGDAN - MIHAI
//313CB
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "structs.h"
#include "functiiaux.h"
#define MaxSize 3145728

int main(int argc, char *argv[])
{
    if(argc != 3){
        printf("Argumente invalide!\n");
    }
	FILE *in, *out;
	in = fopen(argv[1], "rt");
	out = fopen(argv[2],"wt");
	if(!in) return -1;
	if(!out) return -1;
	int cuanta;
    fscanf(in,"%d",&cuanta);
    fgetc(in); // scap de \n
    //citesc din fisier linie cu linie
    TListaMem l = NULL;
    int k, nr = 0, adresa = 0, nrELemQ = 0;
    PID a[1000];
    //creez un vector in care vom retine pidurile disponibile
    for(k = 0; k < 1000; k++)
    {
    	a[k].pid = k+1;
    	a[k].valabilitate = 1;
    }
    TCoada *c_waiting, *c_running, *c_finished;
    c_waiting = InitQ(sizeof(TProcess));
    c_running = InitQ(sizeof(TProcess));
    c_finished = InitQ(sizeof(TProcess));

    int total_mem_size = 0;
    //citesc din fisier linie cu linie
    char buffer[1000];
    while(fgets(buffer, 1000, in))
    {
        buffer[strlen(buffer) - 1] = '\0';
    	char *operatie = strtok(buffer, " ");
    	if(strcmp(operatie, "add") == 0)
    	{

    		nr++;
    		int i;
            //extrag parametrii
    		char *mem_size = strtok(NULL, " "),  *exec_time = strtok(NULL," "), *priority = strtok(NULL, " ");
    		TProcess *p = AlocaProces(), *q = AlocaProces();
            //caut primul pid disponibil
    		for(i = 0; i < 300; i++)
    		{
    			if(a[i].valabilitate == 1)
    			{
    				a[i].valabilitate = 0;
    				p->PID = a[i].pid;
                    break;
    			}
    		}
            //atribui valorile citite campurilor procesului
            p->cuanta = cuanta;
    		p->priority = atoi(priority);
    		strcpy(p->stare, "waiting");
    		p->exec_time = p->time = atoi(exec_time);
    		p->mem_size = p->stack_size =  atoi(mem_size);
            total_mem_size += atoi(mem_size);
            //adaug procesul in memorie
            int var = AddInMem(&l, atoi(mem_size), p->PID);
            //daca are loc in memorie il adaug in coada de asteptare
            //in caz contrar afisez mesajul cerut, iar apoi apelez functia
            //de defragmentare
            if(var != -1)
            {
                p->adresa = var;

        		add(c_waiting, p);
                

                if(c_running->ic == NULL)
        		{
        			ExtrQ(c_waiting,q);
        			IntrQ(c_running, q);
        		}
        		fprintf(out, "Process created successfully: PID: %u, Memory starts at 0x%x.\n",p->PID, p->adresa );
        		adresa += p->mem_size;
        		nrELemQ++;

            }
            else
            {
                fprintf(out, "Cannot reserve memory for PID %d.\n",p->PID );
                a[p->PID - 1].valabilitate = 1;
                defragmentare(&l);
            }
    	}
    	if(strcmp(operatie, "get") == 0)
    	{
    		char *pid = strtok(NULL, " ");
    		get(c_running, c_waiting, c_finished, atoi(pid), out);
    	}
    	if(strcmp(operatie, "push") == 0)
    	{
    		char *pid = strtok(NULL, " ");
    		char *data = strtok(NULL, " ");
            int Data = atoi(data);
    		pushC(atoi(pid), Data, c_waiting, c_running, out);
    	}
    	if(strcmp(operatie, "print") == 0)
    	{
    		char *type = strtok(NULL, " ");
    		if(strcmp(type, "waiting") == 0)
    			print(c_waiting, type, out);
    		if(strcmp(type, "finished") == 0)
    			print(c_finished, type, out);
            if(strcmp(type, "running") == 0)
            {
                print(c_running, type, out);
            }
    		if(strcmp(type, "stack") == 0)
    		{
    			char *pid = strtok(NULL, " ");
    			print_stack(c_waiting, c_running, atoi(pid), out);
    		}
    	}
        if(strcmp(operatie, "run") == 0)
        {
            char *time = strtok(NULL, " ");
            int Time = atoi(time);
            run(Time, cuanta, c_waiting, c_running, c_finished, a, l);
            ElimDuble(l);
        }
        if(strcmp(operatie, "pop") == 0)
        {
            char *pid = strtok(NULL, " ");
            popC(c_waiting, c_running, atoi(pid), out);
        }
        if(strcmp(operatie, "finish") == 0)
        {
            fprintf(out, "Total time: %d\n",finish(c_running, c_waiting));
        }
    }
    
    fclose(in);
   	fclose(out);

	return 0;
}