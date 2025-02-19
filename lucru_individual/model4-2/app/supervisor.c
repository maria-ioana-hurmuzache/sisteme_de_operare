#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <semaphore.h>


void scriere_pipe(int read_fd)
{
    char ch;
    ssize_t bytes_read;
    while((bytes_read=read(read_fd, &ch, sizeof(char)))>0)
    {
        if(-1==(write(STDOUT_FILENO, &ch, sizeof(char))))
        {
            perror("Eroare la scrierea in pipe");
            exit(1);
        }
    }
    ch='\0';
    if(-1==(write(STDOUT_FILENO, &ch, sizeof(char))))
    {
        perror("Eroare la scrierea in pipe");
        exit(1);
    }
    if (bytes_read == -1)
    {
        perror("Eroare la citirea din fisier");
        exit(1);
    }
    if(-1==close(read_fd))
    {
        perror("Eroare la inchidera fisierului");
        exit(1);
    }
}

void citire_shm(void* shm_ptr)
{
    char vocala, consoana;
    int apv, apc;
    vocala=*((char*)shm_ptr);
    apv=*((int*)(shm_ptr+sizeof(char)));
    consoana=*((char*)(shm_ptr+sizeof(char)+sizeof(int)));
    apc=*((int*)(shm_ptr+2*sizeof(char)+sizeof(int)));
    printf("Perechile din memoria partajata sunt:\n%c, %d\n%c, %d\n", vocala, apv, consoana, apc);
}

int main(int argc, char*argv[])
{
    sem_t *sem;
    if(SEM_FAILED==(sem=sem_open("my_sem", 0666, 0)))
    {
        perror("Eroare la deschiderea semaforului");
        exit(1);
    } 
    
    if(argc!=3)
    {
        fprintf(stderr, "Programul accepta un singur argument, calea unui fisier\n");
        exit(1);
    }
    
    int saved_fd=atoi(argv[2]);
    
    int shm_fd;
    void* shm_ptr;
    
    shm_fd=shm_open("myshm", O_RDWR, 0666);
    if(shm_fd==-1)
    {
        perror("Eroare la crearea memoriei partajate");
        exit(1);
    }
    
    if (ftruncate(shm_fd, 2*sizeof(char)+3*sizeof(int))==-1)
    {
        perror("Eroare la trunchierea memoriei partajate");
        exit(1);
    }
    
    shm_ptr=mmap(NULL, 2*sizeof(char)+3*sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr==MAP_FAILED)
    {
        perror("Eroare la mapare");
        exit(1);
    }
    
    *(int*)(shm_ptr+2*(sizeof(char)+sizeof(int)))=0;
    
    int fd=open(argv[1], O_RDONLY);
    if(fd==-1)
    {
        perror("Eroare la open");
        exit(1);
    }
    
    scriere_pipe(fd);
    sem_post(sem);
    sem_close(sem);
    
    while(*((int*)(shm_ptr+2*(sizeof(char)+sizeof(int))))!=1)
    {
        sleep(1);
    }
    
    if(-1==dup2(saved_fd, STDOUT_FILENO))
    {
        perror("Eroare la dup2");
        exit(1);
    }
    
    citire_shm(shm_ptr);
    
    if(-1==(munmap(shm_ptr, 2*sizeof(char)+3*sizeof(int))))
    {
        perror("Eroare la eliminarea maparii memoriei partajate");
        exit(1);
    }
    
    if(-1==shm_unlink("myshm"))
    {
        perror("Eroare la unlink");
        exit(1);
    }
    
    return 0;
}
