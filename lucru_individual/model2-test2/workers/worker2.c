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

void worker1_worker2_supervisor(int read_fd, int* shm_map)
{
    ssize_t bytes_read;
    int nr[2]={0};
    int dif_max=0;
    while((bytes_read=read(read_fd, nr, 2*sizeof(int)))>0)
    {
        if(abs(nr[0]-nr[1])>dif_max)
        {
            shm_map[0]=nr[0];
            shm_map[1]=nr[1];
            dif_max=abs(nr[0]-nr[1]);
        }
    }
    if(-1==bytes_read)
    {
        perror("Eroare la citire");
        close(read_fd);
        munmap(shm_map, 3*sizeof(int));
        exit(1);
    }
    shm_map[2]=1;
    if(-1==close(read_fd))
    {
        perror("Eroare la inchiderea capatului de citire al canalului anonim");
        munmap(shm_map, 3*sizeof(int));
        exit(1);
    }
    if(-1==munmap(shm_map, 3*sizeof(int)))
    {
        perror("Eroare la demapare");
        exit(1);
    }
}

int main()
{
    //creare canal anonim
    int pipe_w1_w2[2];
    if(-1==pipe(pipe_w1_w2))
    {
        perror("Eroare la crearea canalului anonim");
        exit(1);
    }
    
    //cream un proces fiu
    pid_t pid=fork();
    if(pid==-1)
    {
        perror("Eroare la crearea procesului fiu");
        exit(1);
    }
    //salvam descriptorul original pt stdout
    int saved_fd;
    if(-1==(saved_fd=dup(STDOUT_FILENO)))
    {
        perror("Eroare dup");
        exit(1);
    }
    
    else if(pid==0)
    {
        //inchidem capatul de citire
        if(-1==close(pipe_w1_w2[0]))
        {
            perror("Eroare la inchiderea capatului de citire al canalului anonim");
            exit(1);
        }
        //redirectam intrarea standard
        if(-1==dup2(pipe_w1_w2[1], STDOUT_FILENO))
        {
            perror("Eroare la dup2");
            exit(1);
        }
        //pornim procesul worker1 prin execl
        execl("./workers/worker1", "./workers/worker1", NULL);
        fprintf(stderr, "Eroare la functia execl\n");
        exit(1);
    }
    
    //inchidem capatul de scriere
    if(-1==close(pipe_w1_w2[1]))
    {
        perror("Eroare la inchiderea capatului de scriere al canalului anonim");
        exit(1);
    }
    //initializare memorie partajata
    char* shm_name="myshm";
    int shm_fd=shm_open(shm_name, O_CREAT | O_RDWR, 0666);
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
    
    
    wait(NULL);
    if(-1==dup2(saved_fd, STDOUT_FILENO))
    {
        perror("Eroare la dup2");
        exit(1);
    }
    //functie
    worker1_worker2_supervisor(pipe_w1_w2[0], shm_map);
    
    return 0;
}
