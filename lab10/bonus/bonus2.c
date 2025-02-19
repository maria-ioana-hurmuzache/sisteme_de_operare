#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <string.h>

void master_init(int* shared_mem1, int* shared_mem2);
void master_finish(int* shared_mem1, int* shared_mem2);
void slave_work(int* shared_mem, int index);

int main()
{
    int pid1, pid2;

    //maparea memoriei partajate pentru comunicare
    int* shared_mem1 = mmap(NULL, sizeof(int) * 100, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    int* shared_mem2 = mmap(NULL, sizeof(int) * 100, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);

    if (shared_mem1 == MAP_FAILED || shared_mem2 == MAP_FAILED) 
    {
        perror("Eroare la mapare");
        exit(EXIT_FAILURE);
    }

    memset(shared_mem1, 0, sizeof(int) * 100);
    memset(shared_mem2, 0, sizeof(int) * 100);

    master_init(shared_mem1, shared_mem2);

    //primul proces
    if (-1 == (pid1 = fork())) 
    {
        perror("Eroare la fork pentru fiul #1");
        exit(1);
    }

    if (pid1 == 0) 
    { 
        slave_work(shared_mem1, 0);
        return 0;
    }
    //al doilea proces
    if (-1 == (pid2 = fork())) 
    {
        perror("Eroare la fork pentru fiul #2");
        exit(2);
    }

    if (pid2 == 0) 
    { 
        slave_work(shared_mem2, 1);
        return 0;
    }

    //astept sa termine fii
    wait(NULL);
    wait(NULL);
    
    //sume partiale
    master_finish(shared_mem1, shared_mem2);

    //demapez
    munmap(shared_mem1, sizeof(int) * 100);
    munmap(shared_mem2, sizeof(int) * 100);

    return 0;
}

void master_init(int* shared_mem1, int* shared_mem2)
{
    int nr, flag;
    int index1 = 0, index2 = 0;

    printf("Introduceti numerele (0 pentru terminare):\n");
    flag = 1;
    do
    {
        scanf("%d", &nr);
        if (flag == 1) 
            shared_mem1[index1++] = nr;
        else 
            shared_mem2[index2++] = nr;
        flag = 3 - flag;
    } while (nr != 0);
}

void master_finish(int* shared_mem1, int* shared_mem2)
{
    int sp1 = shared_mem1[0];
    int sp2 = shared_mem2[0];

    //suma totala
    printf("[Master PID=%d] Suma numerelor introduse este: %d\n", getpid(), sp1 + sp2);
}

void slave_work(int* shared_mem, int index)
{
    int suma_partiala = 0;
    int i = 0;
    while (shared_mem[i] != 0)
    {
        suma_partiala += shared_mem[i++];
    }

    //scriu suma partiala in mem partajata
    shared_mem[0] = suma_partiala;

    printf("[Slave PID=%d] Suma partiala: %d\n", getpid(), suma_partiala);
}
