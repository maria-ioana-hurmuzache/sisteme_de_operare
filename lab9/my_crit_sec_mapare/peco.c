#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <semaphore.h>
#include <errno.h>
#include <string.h>

// Numele semaforului
#define SEMAPHORE_NAME "/semafor_peco"

void process_operations(int num_operations, char *operations[])
{
    // Deschid semaforul POSIX
    sem_t *sem = sem_open(SEMAPHORE_NAME, O_CREAT, 0644, 1);
    if (sem == SEM_FAILED)
    {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    // Deschid fișierul binar și mapez în memorie
    int fd = open("peco.bin", O_RDWR);
    if (fd == -1)
    {
        perror("open");
        sem_close(sem);
        exit(EXIT_FAILURE);
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1)
    {
        perror("fstat");
        sem_close(sem);
        close(fd);
        exit(EXIT_FAILURE);
    }

    char *map_addr = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map_addr == MAP_FAILED)
    {
        perror("mmap");
        sem_close(sem);
        close(fd);
        exit(EXIT_FAILURE);
    }
    
    for (int i = 0; i < num_operations; i++)
    {
        
        double value = atof(operations[i]);

        // Intru în secțiunea critică
        if (sem_wait(sem) == -1)
        {
            perror("sem_wait");
            munmap(map_addr, sb.st_size);
            sem_close(sem);
            close(fd);
            exit(EXIT_FAILURE);
        }
        
        double rezervor=0;
    
    for(size_t i=0; map_addr[i]!='\n' && i<sb.st_size; i++)
    {
        while(map_addr[i]!='.' && map_addr[i]!='\n' && i<sb.st_size)
        {
            rezervor=rezervor*10+map_addr[i]-'0';
            i++;
        }
        if(map_addr[i]=='.')
        {
            i++;
            double p10=0.1;
            while(map_addr[i]!='\n' && i<sb.st_size)
            {
                rezervor=rezervor+p10*(map_addr[i]-'0');
                p10*=0.1;
                i++;
            }
        }
    }
        
        // Execută operația
        if(rezervor+value>=0)
        {
            rezervor += value;
        }
        else
        {
            printf("Extragerea este prea mare, deci nu se realizeaza: ");
        }
        printf("PID: %d - %s %.2f - Rezervor: %.2f\n", getpid(), (value > 0 ? "Depunere" : "Extragere"), value, rezervor);
        char buffer[50];
        sprintf(buffer, "%.10f", rezervor);
        if(strlen(buffer)>sb.st_size)
        {
            strncpy(map_addr, buffer, sb.st_size);
        }
        else
        {
            strcpy(map_addr, buffer);
        }
        
        if (msync(map_addr, sb.st_size, MS_SYNC) == -1)
        {
            perror("msync");
            munmap(map_addr, sb.st_size);
            close(fd);
            exit(EXIT_FAILURE);
        }

        // Ies din secțiunea critică
        if (sem_post(sem) == -1)
        {
            perror("sem_post");
            munmap(map_addr, sb.st_size);
            sem_close(sem);
            close(fd);
            exit(EXIT_FAILURE);
        }
    }

    munmap(map_addr, sb.st_size);
    close(fd);
    sem_close(sem);
}

int main(int argc, char *argv[]) {
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <operation1> <operation2> ...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    process_operations(argc - 1, &argv[1]);

    return 0;
}

