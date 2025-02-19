#include <stdio.h>    /*input and output functions printf(), scanf()...*/
#include <stdlib.h>   /*memory allocation, conversions, sort : atoi(), sort(), free(), malloc()*/
#include <unistd.h>   /*POSIX functions, dup(), fork(), exec(), pipe(), read(), write()*/
#include <sys/stat.h> /*info about files: stat(), chmod(), mkdir(), rmdir()*/
#include <sys/types.h>/*data types, pid_t, size_t, time_t, mode_t*/
#include <fcntl.h>    /*file control operations open(), fcntl(), creat(), */
#include <errno.h>    /*variable errno - error type*/
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>

///lungime fixa - text (1 octet, 2 octeti, 3 octeti...etc) // binar (4 octeti-lungime fixa)


#define ERROR_CHECK(condition, msg, cod) \
    if((condition))                      \
    {                                    \
        perror(msg);                     \
        exit(cod);                       \
    }                                    \
 
#define MAX_LEN 1024

void prelucrare_fisier(FILE * file, int fd)
{
    char ch;
    while (EOF != fscanf(file, "%c", &ch))
    {
        if ((ch >= 'A' && ch <= 'Z') || (ch >='a' && ch <= 'z'))
           write(fd, &ch, sizeof(char));
    }

    //printf("Trimis catre worker1: %c\n", ch);
    //fflush(stdout);

    /*inchidem desccriptorul de scriere din pipe*/
    close(fd);
}

void get_from_mapp(int shm_fd)
{
    /*mapam obicetul in memorie*/
    int *map_adrr = mmap(NULL, 2 * sizeof(int), PROT_READ | PROT_WRITE,MAP_SHARED,shm_fd,0);
    ERROR_CHECK(map_adrr == MAP_FAILED,  "Eroare la mapare in supervisor", 2); 
    
    int *p_nr = NULL;
    int vocale, consoane;
    p_nr = (int *) map_adrr;
    vocale = *p_nr; 
    p_nr++;
    consoane = *p_nr;

    // int vocale = map_adrr[0];
    // int consoane = map_adrr[1];
    
    printf("Numarul de vocale este : %d \nNumarul de consoane este : %d \n", vocale, consoane);
    ERROR_CHECK(munmap(map_adrr, 2 * sizeof(int)) == -1, "Eroare la demapare", 3);
}


int main(int argc, char* argv[])
{
   // ERROR_CHECK();
    int shm_fd, pid1;
    int pipe_P0toP1[2];
    if (argc != 2)
    {
        printf("Nr de argumente insuficiente");
        exit(0);
    }
    
    /*decshiderea fiseirului de la intrare*/
    FILE* file = fopen(argv[1], "r");
    ERROR_CHECK(NULL == file,  "Eroare la dechiderea fisierului primit ca argument", 0); 
    
    /*crearea maparii neperesistente*/
    const char* shm_name = "comunicare_decriptie";
    shm_fd = shm_open(shm_name, O_RDWR, 0666);
    
    ERROR_CHECK(shm_fd == -1,  "Eroare la crearea obiectului de memorie partajata", 1);  
    ERROR_CHECK(ftruncate(shm_fd, 2*sizeof(int)) == -1,  "Eroare la trunchierea obiectului de memorie partajata", 2);

    /*creaarea pipeului de comunicatie catre P1*/
    ERROR_CHECK(pipe(pipe_P0toP1) == -1,  "Eroare la crearea pipe_P0toP1", 3);
    
    /* crearea procesului fiu care comunica cu P1 prin pipe*/
    ERROR_CHECK(-1 == (pid1 = fork()),  "Eroare la crearea procesului fiu 1", 4);
    if (pid1 == 0)
    {
        ERROR_CHECK(-1 == close(pipe_P0toP1[1]),  "Eroare la inchiderea capatului de scriere in pipe", 5);
        
        /*duplicam intrarea std in procesul fiu 1*/
        ERROR_CHECK(-1 == dup2 (pipe_P0toP1[0], STDIN_FILENO),  "Eroare la redirectarea intrarii std in proces fiu 1", 6);
        
        /*apelul exec*/
        execl("../slaves/worker1", "../slaves/worker1", NULL);
        
        printf("Eroare la apelul exec in proces supervisor");
        exit(EXIT_FAILURE);
    }
    
    /*inchiderea capatului de citire din pipe in proces supervisor*/
    ERROR_CHECK(-1 == close (pipe_P0toP1[0]),  "Eroare la inchiderea capatului de citire in proces supervisor", 7);
    
    /*prelucrare fisier*/
    prelucrare_fisier(file, pipe_P0toP1[1]);
    wait(NULL);

    /*preluam datele din obiectul mapat in memorie*/
    get_from_mapp(shm_fd);
    fclose(file);
    shm_unlink(shm_name);
    return 0;
}