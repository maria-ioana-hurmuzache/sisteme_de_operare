#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#define BUFFER_SIZE 4096

int main(int argc, char* argv[])
{
    if (argc!=2)
    {
        fprintf(stderr, "Programul nu a primit un numar corespunzator de parametri\n");
        return 1;
    }
    
    struct stat st;
    if(-1==(stat(argv[1],&st)))
    {
        fprintf(stderr, "Probleme la functia stat\n");
        return 2;
    }
    
    if((st.st_mode & S_IFMT)!=S_IFREG)
    {
        fprintf(stderr, "Parametrul introdus nu este un fiser valid\n");
        return 3;
    }
    
    int fd=open(argv[1],O_RDONLY);
    if(fd==-1)
    {
        fprintf(stderr, "Eroare la deschiderea fisierului\n");
        return 4;
    }
    
    long int nrcuv=0, nrlinii=0, nrcaractere=0;
    ssize_t bytes_read;
    char buffer[BUFFER_SIZE]; 
    while ((bytes_read=read(fd, buffer, BUFFER_SIZE))>0)
    {
        for(ssize_t i=0; i<bytes_read; i++)
        {
            nrcaractere++;
            if(buffer[i]==' ')
            {
                nrcuv++;
            }
            else if(buffer[i]=='\n')
            {
                nrlinii++;
            }
        }
    }
    printf("%ld cuvinte %ld linii %ld caractere\n",nrcuv,nrlinii,nrcaractere);
    close(fd);
}
