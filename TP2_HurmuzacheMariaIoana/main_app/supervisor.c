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
#include <ctype.h>

void sup2w1(int fd, void*shm_map)
{
    ssize_t bytes_read;
    char buffer[256];
    ssize_t size=sizeof(char);
    char chestii[]="0123456789abcdefghijklmnopqrstuvwxyz";
    while((bytes_read=read(fd, buffer, sizeof(buffer)))>0)
    {
        for(int i=0; buffer[i]!='\n'; i++)
        {
            if(strchr(chestii,buffer[i])!=NULL)
            {
                *((char*)shm_map+size)=buffer[i];
                size+=sizeof(char);
            }
        }
    }
    if(-1==bytes_read)
    {
        perror("Eroare la citire");
        exit(1);
    }
    *((char*)shm_map+size)='\0';
    size+=sizeof(char);
    if(-1==close(fd))
    {
        perror("Eroare la inchidere");
        exit(1);
    }
    
}

void w12sup(void* shm_map)
{
    int nr1=*((int*)shm_map+sizeof(int));
    int nr2=*((int*)shm_map+2*sizeof(int));
    printf("Suma este %d\n", nr1+nr2);
}

int main(int argc, char*argv[])
{
    if(argc!=2)
    {
        fprintf(stderr, "Programul primeste un singur argument\n");
        exit(1);
    }
    char* filename=argv[1];
    
    char* shm_name="shm_open";
    int shm_fd=shm_open(shm_name, O_CREAT|O_RDWR, 0666);
    if(-1==shm_fd)
    {
        perror("Eroare la crearea obiectului de memorie partajata in supervisor");
        exit(1);
    }
    if(-1==ftruncate(shm_fd, 4096))
    {
        perror("Eroare la functia ftruncate");
        exit(1);
    }
    void* shm_map=mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(shm_map==MAP_FAILED)
    {
        perror("Eroare la mapare");
        exit(1);
    }
    
    int fd=open(filename, O_RDONLY);
    if(-1==fd)
    {
        perror("Eroare la deschiderea fisierului");
        exit(1);
    }
    
    sup2w1(fd, shm_map);
    *((char*)shm_map)='r';
    while(*((int*)(shm_map))!=1)
    {
        sleep(1);
    }
    w12sup(shm_map);
    if(-1==munmap(shm_map, 4096))
    {
        perror("Eroare la demapare");
        exit(1);
    }
    
    return 0;
}
