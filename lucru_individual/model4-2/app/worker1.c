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
#include <signal.h>

void calculeaza_si_scrie(int read_fd, int write_fd)
{
    int frecventa[256]={0};
    char ch;
    int i;
    ssize_t bytes_read;
    while((bytes_read=read(read_fd, &ch, sizeof(char)))>0)
    {
        if(ch=='\0')
        {
            break;
        }
        i=(int) ch;
        frecventa[i]++;
    }
    if (bytes_read == -1)
    {
        perror("Eroare la citirea din pipe");
        exit(1);
    }
    if(-1==close(read_fd))
    {
        perror("Eroare la inchiderea capatului de citire w1");
        close(write_fd);
        exit(1);
    }
    for(size_t i=0; i<256; i++)
    {
        if(frecventa[i]>0)
        {
            ch=(char)i;
            if(-1==write(write_fd, &ch, sizeof(char)))
            {
                perror("Eroare la scriere");
                exit(1);
            }
            if(-1==write(write_fd, &frecventa[i], sizeof(int)))
            {
                perror("Eroare la scriere");
                exit(1);
            }
        }
    }
    if(-1==close(write_fd))
    {
        perror("Eroare la inchidere fifo");
        exit(1);
    }
}

int main(int argc, char*argv[])
{
    if (argc!=2)
    {
        fprintf(stderr, "Nu ati intordus niciun fisier!\n");
        exit(1);
    }
    char* filename=argv[1];
    
    int pipe_sup_w1[2];
    if(-1==pipe(pipe_sup_w1))
    {
        perror("Eroare la pipe");
        exit(1);
    }
    
    sem_t *sem;
    if(SEM_FAILED==(sem=sem_open("my_sem", O_CREAT, 0666, 0)))
    {
        perror("Eroare la deschiderea semaforului");
        exit(1);
    }
    
    pid_t pid1;
    if (-1==(pid1=fork()))
    {
        perror("Eroare la fork");
        exit(1);
    }
    if (pid1==0)
    {
        if(-1==close(pipe_sup_w1[0]))
        {
            perror("Eroare la inchiderea capatului de citire pipe");
            exit(1);
        }
        int saved_fd;
        char saved[10];
        if(-1==(saved_fd=dup(STDOUT_FILENO)))
        {
            perror("Eroare dup");
            exit(1);
        }
        snprintf(saved, sizeof(saved), "%d", saved_fd);
        
        if(-1==dup2(pipe_sup_w1[1], STDOUT_FILENO))
        {
            perror("Eroare la redirectarea stdout");
            exit(1);
        }
        
        execl("./app/supervisor", "./app/supervisor", filename, saved, NULL);
        perror("Eroare la execl");
        exit(1);
    }
    
    if(-1==close(pipe_sup_w1[1]))
    {
        perror("Eroare la inchiderea capatului de scriere");
        exit(1);
    }
    
    if(mkfifo("fifo1_2",0666)==-1)
    {
        if(errno!=EEXIST)
        {
            perror("Eroare la crearea fifo-ului");
            exit(1);
        }
    }
    
    int fifo_fd=open("fifo1_2", O_WRONLY);
    if(fifo_fd==-1)
    {
        perror("Eroare la deschiderea fifo-ului");
        exit(1);
    }
    
    sem_wait(sem);
    sem_close(sem);
    sem_unlink("my_sem");
    calculeaza_si_scrie(pipe_sup_w1[0], fifo_fd);
    
    wait(NULL);
    
    return 0;
}
