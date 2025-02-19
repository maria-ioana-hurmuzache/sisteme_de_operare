#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    pid_t pid;
    int status;
    char* cut_args[]={"cut", "--fields", "1,3,4", "-d:", "--output-delimiter= -- ", "/etc/passwd", NULL };
    
    if(-1==(pid=fork()))
    {
        perror("Eroare la fork");
        exit(1);
    }
    
    if(pid==0)
    {
        execvp("cut", cut_args);
        perror("Eroare la execvp");
        exit(14);
    }
    wait(&status);
    if(WIFEXITED(status))
    {
        switch( WEXITSTATUS(status) )
        {
            case 14:  printf("Comanda cut nu a putut fi executata... (apelul exec a esuat).\n");  break;
            case  0:  printf("Comanda cut a fost executata cu succes!\n");  break;
            default:  printf("Comanda cut a fost executata, dar a esuat, terminandu-se cu codul de terminare: %d.\n", WEXITSTATUS(status) );
        }
    }
    else
    {
        printf("Comanda cut a fost terminata fortat de catre semnalul: %d.\n", WTERMSIG(status) );
    }
    return 0;
}
