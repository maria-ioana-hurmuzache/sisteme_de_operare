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

void procesare(int fifo_fd, int shm_fd)
{
    int* shm_map=mmap(NULL, 3*sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(shm_map==MAP_FAILED)
    {
        perror("Eroare la mapare");
        exit(1);
    }
    
    const char vocale[]="AEIOUaeiou";
    int bytes_read, cntV=0, cntC=0;
    char ch;
    
    while((bytes_read=read(fifo_fd, &ch, sizeof(char)))>0)
    {
        if(strchr(vocale, ch)!=NULL)
        {
            //printf("%c\n", ch);
            cntV++;
        }
        else
        {
            //printf("%c\n", ch);
            cntC++;
        }
    }
    if(bytes_read==-1)
    {
        perror("Eroare la citirea din fifo");
        exit(1);
    }
    shm_map[0]=cntV;
    shm_map[1]=cntC;
    shm_map[2]=1;
    //actualizarea memoriei partajate
    if(-1==msync(shm_map, 3*sizeof(int), MS_SYNC))
    {
        perror("Eroare la msync");
        close(fifo_fd);
        exit(1);
    }
    if(-1==munmap(shm_map, 3*sizeof(int)))
    {
        perror("Eroare la demapare");
        close(fifo_fd);
        close(shm_fd);
        exit(1);
    }
    if(-1==close(fifo_fd))
    {
        perror("Eroare la inchiderea capatului de citire al fifo-ului");
        close(shm_fd);
        exit(1);
    }
    if(-1==close(shm_fd))
    {
        perror("Eroare la inchiderea memoriei partajate");
        exit(1);
    }
}

int main()
{
    //memoria partajata
    const char* shm_name="comunicare_decriptare";
    int shm_fd=shm_open(shm_name, O_CREAT|O_RDWR, 0666);
    if(-1==shm_fd)
    {
        perror("Eroare la deschiderea memoriei partajate");
        exit(1);
    }
    if(-1==ftruncate(shm_fd, 3*sizeof(int)))
    {
        perror("Eroare la ftruncate");
        exit(1);
    }
    
    //deschiderea fifo-ului
    int fifo_fd;
    if(-1==(fifo_fd=open("../send_data", O_RDONLY)))
    {
        perror("Eroare la deschiderea fifo");
        exit(1);
    }
    
    procesare(fifo_fd, shm_fd);
    
    return 0;
}
