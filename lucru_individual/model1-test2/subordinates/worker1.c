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

void worker1_to_worker2(int fifo_fd, int write_fd)
{
    ssize_t bytes_read;
    struct operatie o;
    int rezultat=0;
    while((bytes_read=read(fifo_fd, &o, sizeof(struct operatie)))>0)
    {
        switch(o.operator)
        {
            case '+':
            {
                rezultat=o.operanzi[0]+o.operanzi[1];
                if(-1==write(write_fd, &rezultat, sizeof(int)))
                {
                    perror("Eroare la scrierea in canalul anonim");
                    exit(1);
                }
                break;
            }
            case '-':
            {
                rezultat=o.operanzi[0]-o.operanzi[1];
                if(-1==write(write_fd, &rezultat, sizeof(int)))
                {
                    perror("Eroare la scrierea in canalul anonim");
                    exit(1);
                }
                break;
            }
            case '*':
            {
                rezultat=o.operanzi[0]*o.operanzi[1];
                if(-1==write(write_fd, &rezultat, sizeof(int)))
                {
                    perror("Eroare la scrierea in canalul anonim");
                    exit(1);
                }
                break;
            }
            case '/':
            {
                rezultat=o.operanzi[0]/o.operanzi[1];
                if(-1==write(write_fd, &rezultat, sizeof(int)))
                {
                    perror("Eroare la scrierea in canalul anonim");
                    exit(1);
                }
                break;
            }
        }
        printf("%d\n", rezultat);
    }
    if(bytes_read==-1)
    {
        perror("Eroare la citirea din fifo");
        exit(1);
    }
    if(-1==close(write_fd))
    {
        perror("Eroare la inchiderea capatului de scriere al canalului anonim");
        close(fifo_fd);
        exit(1);
    }
    if(-1==close(fifo_fd))
    {
        perror("Eroare la inchiderea capatului de citire al fifo-ului");
        exit(1);
    }
}

int main()
{
    //creare canal anonim
    int pipe_w1w2[2];
    if(-1==pipe(pipe_w1w2))
    {
        perror("Eroare la crearea canalului anonim");
        exit(1);
    }
    
    //initializare fifo
    if(-1==mkfifo("../myfifo", 0666))
    {
        if(errno!=EEXIST)
        {
            perror("Eroare la crearea fifo-ului in supervisor");
            exit(1);
        }
    }
    int fifo_fd=open("../myfifo", O_RDONLY);
    if(-1==fifo_fd)
    {
        perror("Eroare la deschiderea capatului de citire al fifo-ului");
        exit(1);
    }
    
    //crearea procesului fiu
    pid_t pid;
    if(-1==(pid=fork()))
    {
        perror("Eroare la crearea procesului fiu in worker1");
        exit(1);
    }
    if(pid==0)
    {
        if(-1==close(pipe_w1w2[1]))
        {
            perror("Eroare la inchiderea capatului de scriere al canalului anonim worker1-fiu");
            exit(1);
        }
        if(-1==dup2(pipe_w1w2[0], STDIN_FILENO))
        {
            perror("Eroare la redirectarea intrarii standard");
            exit(1);
        }
        //apel exec
        execl("./subordinates/worker2", "./subordinates/worker2", NULL);
        fprintf(stderr, "Eroare la apelul functiei execl\n");
        exit(1);
    }
    if(-1==close(pipe_w1w2[0]))
    {
        perror("Eroare la inchiderea capatului de citire al canalului anonim");
        exit(1);
    }
    
    worker1_to_worker2(fifo_fd, pipe_w1w2[1]);
    
    wait(NULL);
    
    return 0;
}
