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

struct operatie
{
    int operanzi[2];
    char operator;
};

void send2worker1(int fd, int fifo_fd)
{
    ssize_t bytes_read;
    char ch;
    bool i=0;
    struct operatie o={0};
    char operatori[]="+-*/";
    while((bytes_read=read(fd, &ch, sizeof(char)))>0)
    {
        if(strchr(operatori,ch)!=NULL)
        {
            o.operator=*strchr(operatori, ch);
            i=1;
        }
        else if(ch=='\n')
        {
            if(-1==write(fifo_fd, &o, sizeof(struct operatie)))
            {
                perror("Eroare la scrierea in fifo");
                close(fd);
                close(fifo_fd);
                exit(1);
            }
            o.operanzi[0]=0;
            o.operanzi[1]=0;
            i=0;
        }
        else
        {
            o.operanzi[i]=o.operanzi[i]*10+(ch-'0');
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
        perror("Eroare la inchiderea capatului de scriere al fifo-ului");
        exit(1);
    }
}

void get_from_worker2(int* shm_map)
{
    printf("Suma este: %d\n", shm_map[0]+shm_map[1]);
    if(-1==munmap(shm_map, 3*sizeof(int)))
    {
        perror("Eroare la demapare");
        exit(1);
    }
}

int main(int argc, char*argv[])
{
    if(argc!=2)
    {
        fprintf(stderr, "Programul accepta un singur argument, numele fisierului");
        exit(1);
    }
    char* filename=argv[1];
    int fd=open(filename, O_RDONLY);
    if(-1==fd)
    {
        perror("Eroare la deschiderea fisierului primit ca argument");
        exit(1);
    }
    
    //initializare fifo
    int fifo_fd=open("../myfifo", O_WRONLY);
    if(-1==fifo_fd)
    {
        perror("Eroare la deschiderea capatului de scriere al fifo-ului");
        exit(1);
    }
    
    //initializarea memoriei partajate
    const char* shm_name="myshm";
    int shm_fd=shm_open(shm_name, O_CREAT|O_RDWR, 0666);
    if(-1==shm_fd)
    {
        perror("Eroare la crearea obiectului de memorie partajata in supervisor");
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
    
    //comunicarea cu worker1
    send2worker1(fd, fifo_fd);
    
    //comunicare cu worker2
    while(shm_map[2]==0)
    {
        sleep(1);
    }
    get_from_worker2(shm_map);
    if(-1==shm_unlink(shm_name))
    {
        perror("Eroare la unlink");
        exit(1);
    }
    
    return 0;
}
