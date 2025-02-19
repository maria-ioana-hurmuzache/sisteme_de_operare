#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#define BUFFER_SIZE 4096

int N;
char *filename;
int fd;
pid_t pid;

void handle_alarm(int sig)
{
    static int counter = 0;
    counter++;
    if (counter == 5)
    {
        printf("Tatal: Timpul a expirat. Terminare program.\n");
        close(fd);
        kill(pid, SIGUSR2);
        wait(NULL);
        printf("Tatal: sfarsit executie!\n");
        exit(0);
    }
    else
    {
        printf("Tatal: Atentie! Nu s-a primit niciun input.\n");
        alarm(N);
    }
}

void handle_sigusr2(int sig)
{
    printf("\nFiul: am primit USR2!\n");

    char buffer[BUFFER_SIZE];
    int bytes_read;

    int file;
    if (-1==(file=open(filename, O_RDONLY)))
    {
        perror("Fiul: Eroare la deschiderea fisierului");
        exit(1);
    }

    while ((bytes_read = read(file, buffer, sizeof(buffer)-1)) > 0)
    {
        buffer[bytes_read] = '\0';
        printf("%s", buffer);
    }
    
    if(-1==bytes_read)
    {
        perror("Fiul: Eroare la citirea din fisier");
        exit(1);
    }

    close(file);
    printf("\nFiul: sfarsit executie!\n");
    exit(0);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Nu ati introdus toate argumentele!\n");
        exit(1);
    }

    filename = argv[1];
    N = atoi(argv[2]);

    signal(SIGALRM, handle_alarm);
    alarm(N);

    if (-1==(pid = fork()))
    {
        perror("Eroare la fork");
        exit(1);
    }

    if (pid == 0)
    {  // Proces fiu
        signal(SIGUSR2, handle_sigusr2);
        pause();
    }
    else
    {  // Proces părinte
        if (-1==(fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666)))
        {
            perror("Eroare la deschiderea fisierului");
            kill(pid, SIGKILL);
            exit(1);
        }

        char buffer[BUFFER_SIZE];
        ssize_t bytes_read;

        while ((bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0)
        {
            if (write(fd, buffer, bytes_read) == -1)
            {
                perror("Eroare la scrierea in fisier");
                close(fd);
                kill(pid, SIGKILL);  // Omoară procesul fiu
                exit(1);
            }
            alarm(N);  // Resetează timer-ul pentru alarme
        }

        close(fd);
        kill(pid, SIGUSR2);  // Trimite semnalul către fiu
        wait(NULL);  // Așteaptă terminarea procesului fiu
        printf("Tatal: sfarsit executie!\n");
    }

    return 0;
}

