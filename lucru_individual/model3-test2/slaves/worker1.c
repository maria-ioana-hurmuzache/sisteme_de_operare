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

void procesare_canal_anonim(int write_fd)
{
    ssize_t bytes_read;
    char ch, rezultat;
    while((bytes_read=(read(STDIN_FILENO, &ch, sizeof(char)))>0))
    {
        if(ch>='A' && ch<='Z')
        {
            rezultat=(ch-'A'+14)%26+'A';
        }
        else if(ch>='a' && ch<='z')
        {
            rezultat=(ch-'a'+14)%26+'a';
        }
        fflush(stdout);
        if(-1==write(write_fd, &rezultat, sizeof(char)))
        {
            perror("Eroare la scrierea in fifo");
            close(write_fd);
            exit(1);
        }
    }
    if(-1==bytes_read)
    {
        perror("Eroare la citire");
        close(write_fd);
        exit(1);
    }
    if(-1==close(write_fd))
    {
        perror("Eroare la inchiderea fifo-ului");
        exit(1);
    }
}

int main()
{
    //creare fifo
    const char* fifo="../send_data";
    if(-1==mkfifo(fifo, 0666))
    {
        if(errno!=EEXIST)
        {
            perror("Eroare la crearea fifo");
            exit(1);
        }
    }
    //deschidere fifo
    int fifo_fd=open(fifo, O_WRONLY);
    if(fifo_fd==-1)
    {
        perror("Eroare la deschiderea fifo");
        exit(1);
    }
    
    fflush(stdout);
    
    procesare_canal_anonim(fifo_fd);
    
    return 0;
}
