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

void get_from_supervisor(int fd)
{
    int codr;
    char ch, res;
    while(0 != (codr = read(STDIN_FILENO, &ch, sizeof(char))))
    {
        if ((ch >= 'A' && ch <= 'Z')) //if litera mare
        {
            res = (ch - 'A' + 14) % 26 + 'A';
        }
        else
         {
            res = (ch - 'a' + 14) % 26 + 'a';
         }
        
        // Debug: afiseaza caracterul citit si cel transformat
        //printf("Citit: %c, Transformat: %c\n", ch, res);
        fflush(stdout);

         /*scriem in canalul fifo*/
         ERROR_CHECK(-1 == write(fd, &res, sizeof(char)),  "[FIU1]Eroare la scrierea in canalul fifo", 3);
    }
    
}

int main(int argc, char* argv[])
{
    const char* fifo = "../send_data";
    int fd;
    
    if (-1 == mkfifo(fifo, 0666))
    {
       if (errno != EEXIST)
       {
          perror("[FIU1] ERoare la crearea fifo");
          exit(0);
       }
    }
    
    /*dschiderea cpapatului de scriere in fifo, in mod blocant*/
    ERROR_CHECK(-1 == (fd = open(fifo, O_WRONLY)),  "[FIU1] Eroare la deschiderea capatului de scriere in fifo", 1);
    
    // Debug: fifo deschis cu succes
    //printf("[FIU1] FIFO deschis pentru scriere.\n");
    fflush(stdout);

    /*prelucram datele pe care le primim de la supervsior, din stdin*/
    get_from_supervisor(fd);   
    
    ERROR_CHECK(-1 == close(fd),  "[FIU1] Eroare la inchiderea capatului de scriere in fifo", 2);
    return 0;
}