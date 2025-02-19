#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <string.h>

int *shared_mem;
int seq_length;
pid_t pid1, pid2;
int semnal_primit = 0;

void handler1(int sig)
{
    semnal_primit=1;
}

void handler2(int sig)
{
    semnal_primit=2;
}

int main(int argc, char*argv[])
{
    if(argc!=2)
    {
        fprintf(stderr, "Nu ati introdus lungimea secventei de numere\n");
        exit(1);
    }
    seq_length=atoi(argv[1]);
    //cream maparea
    shared_mem=mmap(NULL, seq_length*sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if(shared_mem==MAP_FAILED)
    {
        perror("Eroare la mapare");
        exit(1);
    }
    srand(time(NULL));
    for(int i=0; i<seq_length; i++)
    {
        shared_mem[i]=rand()%100;
    }
    
    for (int i = 0; i < seq_length; i++)
    {
        printf("%d ", shared_mem[i]);
    }
    printf("\n");
    
    pid1=fork();
    
    signal(SIGUSR2, handler1);
    signal(SIGUSR1, handler2);
    
    if(pid1==-1)
    {
        printf("eroare fork\n");
        exit(1);
    }
    if(pid1==0)
    {
        pause();
        if(semnal_primit==1)
        {
            int *vector = (int *)malloc(seq_length * sizeof(int));
            for(int i=0; i<seq_length; i++)
                vector[i]=shared_mem[i];
            for(int i=0; i<seq_length-1; i++)
            {
                for(int j=i+1; j<seq_length; j++)
                {
                    if(vector[i]<vector[j])
                    {
                        int temp=vector[i];
                        vector[i]=vector[j];
                        vector[j]=temp;
                    }
                }
            }
            pause();
            if(semnal_primit==2)
            {
                printf("Secventa sortata descrescator este: ");
                for (int i = 0; i < seq_length; i++)
                {
                    printf("%d ", vector[i]);
                }
                printf("\n");
                exit(0);
            }
        }   
    }
    else
    {   
        pid2=fork();
        if(pid2==-1)
        {
            printf("eroare fork\n");
            exit(1);
        }
        if(pid2==0)
        {
            pause();
            if(semnal_primit==1)
            {
                int xor_sum = 0;
                for (int i = 0; i < seq_length; i++)
                {
                    xor_sum ^= shared_mem[i];
                }
                printf("Suma XOR este: %d\n", xor_sum);
                // Trimite semnalul SIGUSR1 către procesul frate
                kill(pid1, SIGUSR1);
                exit(0);
            }
        }
        else//parinte dupa crearea fiilor
        {
            sleep(1);
            kill(pid1, SIGUSR2);
            kill(pid2, SIGUSR2);
            wait(NULL);
            wait(NULL);
            munmap(shared_mem, seq_length * sizeof(int));
            return 0;
        }
    }
}
