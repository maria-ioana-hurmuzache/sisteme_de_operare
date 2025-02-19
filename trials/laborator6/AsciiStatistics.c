#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 4096
#define ASCII_SET_SIZE 256

unsigned char buffer[BUFFER_SIZE];

int fd, bytes_read, i, count[ASCII_SET_SIZE];

int main(int argc, char* argv[])
{
    if (argc==1)
    {
        fprintf(stderr,"Nu am primit niciun argument.");
        return 1;
    }
    if (-1==(fd=open(argv[1], O_RDONLY)))//conventie de programare
    {
        fprintf(stderr, "Eroare la deschidere.");
        return 2;
    }
    while (0!=(bytes_read=(read(fd, buffer, BUFFER_SIZE))))
    {
        if (-1==bytes_read)
        {
            fprintf(stderr, "Eroare la citire.");
            return 3;
        }
        for(i=0; i<bytes_read; i++)
            count[buffer[i]]++;
    }
    if(close(fd)==-1)
    {
        fprintf(stderr, "Eroare la inchidere.");
        return 4;
    }
    for (i=0; i<ASCII_SET_SIZE; i++)
    {
        if (count[i]!=0)
        {
             printf("Caracterul %c cu codul ascii %d apare de %d ori\n", i, i, count[i]);
        }
    }
}
