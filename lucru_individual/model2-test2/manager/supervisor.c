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

void supervisor2worker1(int fd, int fifo_fd)
{
    ssize_t bytes_read;
    char ch;
    int i=0, nr[2]={0};
    while((bytes_read=read(fd, &ch, sizeof(char)))>0)
    {
        if(ch==' ')
        {
            i=1;
        }
        else if(ch=='\n')
        {
            if(-1==write(fifo_fd, nr, 2*sizeof(int)))
            {
                perror("Eroare la scrierea in fifo");
                exit(1);
            }
            i=0;
            nr[0]=0;
            nr[1]=0;
        }
        else
        {
            nr[i]=nr[i]*10+(ch-'0');
        }
    }
    if(bytes_read==-1)
    {
        perror("Eroare la citirea din fisier");
        exit(1);
    }
    if(-1==close(fd))
    {
        perror("Eroare la inchiderea fisierului");
        close(fifo_fd);
        exit(1);
    }
    if(-1==close(fifo_fd))
    {
        perror("Eroare la inchidere fifo");
        exit(1);
    }
}

void worker2_to_supervisor(int* shm_map)
{
    printf("Produsul este: %d\n", shm_map[0]*shm_map[1]);
    if(-1==munmap(shm_map,3*sizeof(int)))
    {
        perror("Eroare la munmap");
        exit(1);
    }
}

int main(int argc, char* argv[])
{
    if(argc!=2)
    {
        fprintf(stderr, "Programul are nevoie de un argument: calea absoluta/relativa catre un fisier\n");
        exit(1);
    }
    
    //deschidere fisier
    int fd=open(argv[1], O_RDONLY);
    if(-1==fd)
    {
        perror("Eroare la deschiderea fisierului");
        exit(1);
    }
    
    //initializare fifo
    int fifo_fd=open("myfifo", O_WRONLY);
    if(-1==fifo_fd)
    {
        perror("Eroare la deschiderea fifo-ului");
        exit(1);
    }
    
    //initializare memorie partajata
    char* shm_name="myshm";
    int shm_fd=shm_open(shm_name, O_RDWR, 0666);
    if(-1==shm_fd)
    {
        perror("Eroare la deschiderea memoriei partajate");
        exit(1);
    }
    if(-1==ftruncate(shm_fd, 3*sizeof(int)))
    {
        perror("Eroare la functia ftruncate");
        exit(1);
    }
    int* shm_map=mmap(NULL, 3*sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(shm_map==MAP_FAILED)
    {
        perror("Eroare la mapare");
        exit(1);
    }
    shm_map[2]=0; //flag
    
    //apel functie 1
    supervisor2worker1(fd, fifo_fd);
    
    //apel functie 2
    //verific flag
    while(shm_map[2]==0)
    {
        sleep(1);
    }
    worker2_to_supervisor(shm_map);
    
    if(-1==shm_unlink(shm_name))
    {
        perror("Eroare la unlink");
        exit(1);
    }
    
    return 0;
}
