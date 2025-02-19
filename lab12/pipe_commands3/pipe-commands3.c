#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int main()
{
    pid_t pid1, pid2, pid3;
    int pipe12[2], pipe23[2], pipe34[2];
    if(-1==pipe(pipe34))
    {
        fprintf(stderr, "Eroare: nu pot crea al treilea canal anonim, errno=%d\n", errno);
        perror("Cauza este:");
        exit(1);
    }
    if(-1==(pid1=fork()))
    {
        fprintf(stderr, "Eroare: nu pot crea primul fiu, errno=%d\n", errno);
        perror("Cauza este:");
        exit(2);
    }
    if(pid1==0)
    {//fiu
        if(-1==pipe(pipe23))
        {
            fprintf(stderr, "Eroare: nu pot crea al doilea canal anonim, errno=%d\n", errno);
            perror("Cauza este:");
            exit(1);
        }
        if(-1==(pid2=fork()))
        {
            fprintf(stderr, "Eroare: nu pot crea al doilea fiu, errno=%d\n", errno);
            perror("Cauza este:");
            exit(2);
        }
        if(pid2==0)
        {//nepot
            if(-1==pipe(pipe12))
            {
                fprintf(stderr, "Eroare: nu pot crea al treilea canal anonim, errno=%d\n", errno);
                perror("Cauza este:");
                exit(1);
            }
            if(-1==(pid3=fork()))
            {
                fprintf(stderr, "Eroare: nu pot crea al treilea fiu, errno=%d\n", errno);
                perror("Cauza este:");
                exit(2);
            }
            if(pid3==0)
            {//stranepot
                if(-1==dup2(pipe12[1],1))
                {
                    perror("Eroare la redirectarea iesirii standard in al treilea proces fiu a esuat. Cauza:");
                    exit(3);
                }
                execlp("ps", "ps", "-eo", "user,comm,pid", "--no-headers", NULL);
                perror("Eroare la execlp() in al treilea proces fiu. Cauza erorii:");
            }
            else
            {
                close(pipe12[1]);
                if(-1==dup2(pipe12[0],0))
                {
                    perror("Eroare la redirectarea intrarii standard in al doilea fiu a esuat. Cauza:");
                    exit(3);
                }
                if(-1==dup2(pipe23[1],1))
                {
                    perror("Eroare la redirectarea iesirii standard in al doilea proces fiu a esuat. Cauza:");
                    exit(3);
                }
                execlp("tr", "tr", "-s", "\" \"", NULL);
                perror("Eroare la execlp in al doilea proces fiu, Cauza erorii:");
            }
        }
        else
        {
            close(pipe23[1]);
            if(-1==dup2(pipe23[0],0))
            {
                perror("Eroare la redirectarea intrarii standard in primul fiu a esuat. Cauza:");
                exit(3);
            }
            if(-1==dup2(pipe34[1],1))
            {
                perror("Eroare la redirectarea iesirii standard in primul proces fiu a esuat. Cauza:");
                exit(3);
            }
            execlp("cut", "cut", "-d ", "-f1,2,3", "--output-delimiter=:", NULL);
            perror("Eroare la execlp in primul proces fiu, Cauza erorii:");
        }
    }
    else
    {
        close(pipe34[1]);
        if(-1==dup2(pipe34[0],0))
        {
            perror("Eroare la redirectarea intrarii standard in procesul parinte a esuat. Cauza:");
            exit(3);
        }
        execlp("sort", "sort", "-k2", "-t:", NULL);
        perror("Eroare la execlp in procesul parinte, Cauza erorii:");
    }
    return 0;
}
