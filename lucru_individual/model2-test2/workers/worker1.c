#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <fcntl.h>
#include <semaphore.h>

bool coprime(int a, int b)
{
    if(a<b)
    {
        int aux=a;
        a=b;
        b=aux;
    }
    while(b!=0)
    {
        int rest=a%b;
        a=a/b;
        b=rest;
    }
    if(a==1)
    {
        return true;
    }
    return false;
}

void supervisor2worker1_worker2(int fifo_fd)
{
    ssize_t bytes_read;
    int nr[2]={0};
    while((bytes_read=read(fifo_fd, nr, 2*sizeof(int)))>0)
    {
        if(coprime(nr[0], nr[1])==true)
        {
            if(-1==write(STDOUT_FILENO, nr, 2*sizeof(int)))
            {
                perror("Eroare la scrierea in canalul anonim");
                exit(1);
            }
        }
    }
    if(bytes_read==-1)
    {
        perror("Eroare la citirea din fifo");
        exit(1);
    }
    if(-1==close(fifo_fd))
    {
        perror("Eroare la inchidere fifo");
        exit(1);
    }
}

int main()
{
    //initializare fifo
    if(-1==mkfifo("myfifo", 0666))
    {
        if(errno!=EEXIST)
        {
            perror("Eroare la crearea fifo-ului");
            exit(1);
        }
    }
    int fifo_fd=open("myfifo", O_RDONLY);
    if(-1==fifo_fd)
    {
        perror("Eroare la deschidere fifo");
        exit(1);
    }
    
    //functie
    supervisor2worker1_worker2(fifo_fd);
    
    return 0;
}
