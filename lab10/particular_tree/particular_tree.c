#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    int i, j;
    pid_t pid1, pid2, pid3, parinte1, parinte2, parinte3, copil;
    parinte1=getpid();
    pid_t fii1[3]={};
    int coduri_exit1[3]={};
    pid_t fii2[4]={};
    int coduri_exit2[4]={};
    if(-1==(pid1=fork()))
    {
        perror("Eroare la fork");
        exit(1);
    }
    if(0==pid1)
    {
        parinte2=getpid();
        for(i=1; i<=3; i++)
        {
            if(-1==(pid2=fork()))
            {
                perror("Eroare la fork");
                exit(1);
            }
            if(0==pid2)
            {
                parinte3=getpid();
                for(j=1; j<=4; j++)
                {
                    if(-1==(pid3=fork()))
                    {
                        perror("Eroare la fork");
                        exit(1);
                    }
                    if(0==pid3)
                    {
                        copil=getpid();
                        printf("Nr de ordine: P(3, %d), PID: %d, PID parinte: %d\n", (4*(i-1)+j), copil, parinte3);
                        return 0;
                    }
                    else
                    {
                        if(-1==waitpid(pid3, &coduri_exit2[j-1], 0))
                        {
                            perror("Eroare la waitpid");
                            exit(1);
                        }
                        fii2[j-1]=pid3;
                    }
                }
                printf("Nr de ordine: P(2, %d), PID: %d, PID parinte: %d, PID-uri si coduri de terminare fii: ", i, parinte3, parinte2);
                for(j=0; j<4; j++)
                {
                    printf("[%d, %d] ", fii2[j], coduri_exit2[j]);
                }
                printf("\n");
                return 0;
            }
            else
            {
                if(-1==waitpid(pid2, &coduri_exit1[i-1], 0))
                {
                    perror("Eroare la waitpid");
                    exit(1);
                }
                fii1[i-1]=pid2;
            }
        }
        printf("Nr de ordine: P(1, 1), PID: %d, PID parinte: %d, PID-uri si coduri de terminare fii: ", parinte2, parinte1);
        for(i=0; i<3; i++)
        {
            printf("[%d, %d] ", fii1[i], coduri_exit1[i]);
        }
        printf("\n");
        return 0;
    }
    else
    {
        if(-1==waitpid(pid1, NULL, 0))
        {
            perror("Eroare la waitpid");
            exit(1);
        }
    }
    return 0;
}
