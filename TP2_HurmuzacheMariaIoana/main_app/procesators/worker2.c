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

void w2_to_w1(int fifo_fd, int write_fd)
{
    ssize_t bytes_read;
    char ch;
    int transformari=0;
    while((bytes_read=read(fifo_fd, &ch, sizeof(char)))>0)
    {
        if(islower(ch)==false)
        {
            ch=tolower(ch);
            transformari++;
        }
        if(-1==write(write_fd, &ch, sizeof(char)))
        {
            perror("Eroare la scriere");
            exit(1);
        }
    }
    ch='\n';
    if(-1==write(write_fd, &ch, sizeof(char)))
    {
        perror("Eroare la scriere");
        exit(1);
    }
    if(-1==write(write_fd, &transformari, sizeof(int)))
    {
        perror("Eroare la scriere");
        exit(1);
    }
    if(-1==close(fifo_fd))
    {
        perror("Eroare la inchidere");
        close(write_fd);
        exit(1);
    }
    if(-1==close(write_fd))
    {
        perror("Eroare la inchidere");
        exit(1);
    }
}

int main()
{
    int pipe_w2_w1[2];
    if(-1==pipe(pipe_w2_w1))
    {
        perror("Eroare la crearea canalului anonim");
        exit(1);
    }
    pid_t pid=fork();
    if(-1==pid)
    {
        perror("Eroare la crearea fiului");
        exit(1);
    }
    if(pid==0)
    {
        if(-1==close(pipe_w2_w1[1]))
        {
            perror("Eroare la inchiderea capatului de scriere");
            exit(1);
        }
        if(-1==dup2(pipe_w2_w1[0], STDIN_FILENO))
        {
            perror("Eroare la redirectare");
            exit(1);
        }
        execl("./main_app/procesators/worker1", "./main_app/procesators/worker1", NULL);
        perror("Eroare execl");
        exit(1);
    }
    if(-1==close(pipe_w2_w1[0]))
    {
        perror("Eroare la inchiderea capatului de citire");
        exit(1);
    }
    
    if(-1==mkfifo("myfifo", 0666))
    {
        if(errno!=EEXIST)
        {
            perror("Eroare la crearea fifo-ului in supervisor");
            exit(1);
        }
    }
    int fifo_fd=open("myfifo", O_RDONLY);
    if(-1==fifo_fd)
    {
        perror("Eroare la deschiderea capatului de citire al fifo-ului");
        exit(1);
    }
    
    w2_to_w1(fifo_fd, pipe_w2_w1[1]);
    
    return 0;
}
