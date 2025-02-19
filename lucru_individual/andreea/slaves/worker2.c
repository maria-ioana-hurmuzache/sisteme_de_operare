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
#include <stdbool.h>

///lungime fixa - text (1 octet, 2 octeti, 3 octeti...etc) // binar (4 octeti-lungime fixa)


#define ERROR_CHECK(condition, msg, cod) \
    if((condition))                      \
    {                                    \
        perror(msg);                     \
        exit(cod);                       \
    }                                    \
 
#define MAX_LEN 1024

void send_to_supervisor(int fifo, int shm_fd)
{
    /*maparea obicetului in memorie*/
    int *map_adrr = mmap(NULL, 2 * sizeof(int), PROT_READ | PROT_WRITE,MAP_SHARED,shm_fd,0);
    ERROR_CHECK(map_adrr == MAP_FAILED,  "[FIU2] Eroare la mapare", 2); 
    
    char vocale[] = "AEUIOYaeyuio";
    /*citirea caracterolor din fifo*/
    int codr, cnt_vocale = 0, cnt_cons = 0;
    int *p_nr = NULL;
    char ch;

	fflush(stdin);
    
    while(0 != (codr = read(fifo, &ch, sizeof(char))))
    {
        //bool vocala = false;
        for (int i = 0; i < strlen(vocale); i++)
           if (vocale[i] == ch)
           {
             cnt_vocale++;
             break;
           } 
           else { cnt_cons++; break; }
    }

    ERROR_CHECK(codr == -1, "[FIU2] Eroare la citirea din fifo", 3);

    /*scriem in obiectul de mapare cele doua numere astfel calculate*/
    p_nr = (int *) map_adrr;
    *p_nr = cnt_vocale; p_nr++;
    *p_nr = cnt_cons;

    // map_adrr[0] = cnt_vocale;
    // map_adrr[1] = cnt_cons;

    ERROR_CHECK(msync(map_adrr, 2 * sizeof(int), MS_SYNC) == -1, "[FIU2] Eroare la msync", 4);
    ERROR_CHECK(munmap(map_adrr, 2 * sizeof(int)) == -1, "[FIU2] Eroare la demapare", 4);
    
}


int main(int argc, char* argv[])
{
    const char* shm_name="comunicare_decriptie";
    int fifo;
    
    /*initializam comunicarea prin obicetul de memorie partajata*/
    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    ERROR_CHECK(-1 == shm_fd,  "Eroare dechiderea/crearea obiectului de memorie partajata", 0); 
    ERROR_CHECK(-1 == ftruncate(shm_fd, 2*sizeof(int)),  "Eroare la ftruncate", 1); 
    
     /*initializare pentru comunicarea cu canalul fifo de la proces 1*/
    fifo = open("../send_data", O_RDONLY );
    ERROR_CHECK(-1 == fifo,  "Eroare la deschiderea capatului de citire in fifo", 3); 
    
    send_to_supervisor(fifo, shm_fd);
    
    //ERROR_CHECK(-1 == msync(map_adrr, file_length, MS_SYNC),  "Eroare la msync", 1);
    
    //ERROR_CHECK(-1 == munmap(map_adrr, length) ,  "Eroare la munmap", 1);
     close(fifo);
     return 0;
}