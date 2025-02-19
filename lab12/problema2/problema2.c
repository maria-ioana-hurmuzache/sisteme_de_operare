#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>

#define NMAX 1006
#define BUFFER_SIZE 4096

int main()
{
    int pipe_P0toP1[2], pipe_P2toP0[2], p_fifo[2];
    if(-1==pipe(pipe_P0toP1))
    {
        perror("Eroare la crearea primului canal anonim. Cauza:");
        exit(1);
    }
    if(-1==pipe(pipe_P2toP0))
    {
        perror("Eroare la crearea celui de-al doilea canal anonim. Cauza:");
        exit(1);
    }
    if(-1==mkfifo("canal_fifo", 0666))
    {
        if(errno==EEXIST)
        {
            fprintf(stdout, "Nota: canalul fifo exista deja\n");
        }
        else
        {
            perror("Eroare la crearea canalului fifo. Cauza erorii:");
            exit(1);
        }
    }
    
    pid_t pid1, pid2;
    
    if(-1==(pid1=fork()))
    {
        perror("Eroare la crearea primului fiu. Cauza erorii:");
        exit(2);
    }
    if(pid1)
    {
        if(-1==(pid2=fork()))
        {
            perror("Eroare la crearea celui de-al doilea fiu. Cauza erorii:");
            exit(2);
        }
        if(pid2)//parinte dupa crearea celor doi fii
        {
            close(pipe_P0toP1[0]);
            close(pipe_P2toP0[1]);
            int fd;
            if(-1==(fd=open("input_data.txt", O_RDONLY)))
            {
                perror("Eroare la deschiderea fisierului. Cauza:");
                exit(1);
            }
            char buffer[4096];
            ssize_t bytesRead;
            while((bytesRead=read(fd, buffer, BUFFER_SIZE))>0)
            {
                if(-1==(write(pipe_P0toP1[1], buffer, bytesRead)))
                {
                    perror("Eroare la functia write. Cauza:");
                    exit(3);
                }
            }
            
            if(bytesRead==-1)
            {
                perror("Eroare la functia read2. Cauza:");
                close(fd);
                exit(4);
            }
            
            close(fd);
            close(pipe_P0toP1[1]);
            
            wait(NULL);
            wait(NULL);
            
            int count;
            if(read(pipe_P2toP0[0], &count, sizeof(int))==-1)
            {
                perror("Eroare la citirea din al doilea canal anonim. Cauza:");
                exit(5);
            }
            printf("Numarul de cifre hexazecimale distincte este: %d\n", count);
            close(pipe_P2toP0[0]);
            unlink("canal_fifo");
            
        }
        else
        {
            close(pipe_P0toP1[0]);
            close(pipe_P0toP1[1]);
            //close(pipe_P2toP0[1]);
            
            if(-1==(p_fifo[0]=open("canal_fifo", O_RDONLY)))
            {
                perror("Eroare la deschiderea canalului fifo. Cauza:");
                exit(1);
            }
            
            int fd;
            if(-1==(fd=open("statistics.txt", O_WRONLY|O_CREAT|O_TRUNC, 0644)))
            {
                perror("Eroare la deschiderea fisierului. Cauza:");
                exit(1);
            }
            
            int counts[16]={};
            char buffer[BUFFER_SIZE];
            ssize_t bytesRead;
            while((bytesRead=read(p_fifo[0], buffer, BUFFER_SIZE))>0)
            {
                for(ssize_t i=0; i<bytesRead; i++)
                {
                    if(buffer[i]>='0' && buffer[i]<='9')
                    {
                        int index=buffer[i]-'0';
                        counts[index]++;
                    }
                    else
                    {
                        int index=buffer[i]-'A'+10;
                        counts[index]++;
                    }
                }
            }
            
            if(bytesRead==-1)
            {
                perror("Eroare la functia read3. Cauza:");
                close(fd);
                close(p_fifo[0]);
                exit(4);
            }
            
            int count=0;
            
            for(int i=0; i<16; i++)
            {
                if(counts[i]>0)
                {
                    count++;
                    dprintf(fd, "%c: %d\n", (i<10?'0'+i:'A'+(i-10)), counts[i]);
                }
            }
            close(p_fifo[0]);
            close(fd);
            if(-1==(write(pipe_P2toP0[1], &count, sizeof(int))))
            {
                perror("Eroare la scrierea in canalul al doilea. Cauza:");
                exit(3);
            }
            
            close(pipe_P2toP0[1]);
            exit(0);
        }
    }
    else //primul fiu, P1
    {
        close(pipe_P0toP1[1]);
        close(pipe_P2toP0[0]);
        close(pipe_P2toP0[1]);
        
        if(-1==(p_fifo[1]=open("canal_fifo", O_WRONLY)))
        {
            perror("Eroare la deschiderea fisierului fifo. Cauza:");
            exit(1);
        }
        char buffer[BUFFER_SIZE];
        ssize_t bytesRead;
        while((bytesRead=read(pipe_P0toP1[0], buffer, BUFFER_SIZE))>0)
        {
            for(ssize_t i=0; i<bytesRead; i++)
            {
                char ch;
                if((buffer[i]>='0' && buffer[i]<='9') || (buffer[i]>='A' && buffer[i]<='Z'))
                {
                    ch=buffer[i];
                    if(-1==(write(p_fifo[1], &ch, 1)))
                    {
                        perror("Eroare la scrierea in fifo. Cauza:");
                        exit(3);
                    }
                }
                else if(buffer[i]>='a' && buffer[i]<='z')
                {
                    ch=buffer[i]-'a'+'A';
                    if(-1==(write(p_fifo[1], &ch, 1)))
                    {
                        perror("Eroare la scrierea in fifo. Cauza:");
                        exit(3);
                    }
                }
            }
        }
        
        if(bytesRead==-1)
        {
            perror("Eroare la functia read4. Cauza:");
            close(p_fifo[1]);
            close(pipe_P0toP1[0]);
            exit(4);
        }
        
        close(pipe_P0toP1[0]);
        close(p_fifo[1]);
        exit(0);
    }
    return 0;
}
