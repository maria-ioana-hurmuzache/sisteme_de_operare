#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <fcntl.h>
#include <semaphore.h>

void sup2w1(void* shm_map, int fifo_fd)
{
    ssize_t size=sizeof(char);
    char cifre[]="0123456789", ch;
    int sum_digits=0;
    while((ch=*((char*)(shm_map+size)))!='\0')
    {
        if(strchr(cifre, ch))
        {
            sum_digits=sum_digits+(ch-'0');
        }
        else if(-1==write(fifo_fd, &ch, sizeof(char)))
        {
            perror("Eroare la scrierea in canalul anonim");
            exit(1);
        }
    }
    *(int*)(shm_map+sizeof(int))=sum_digits;
}

void w2tow1(void* shm_map)
{
    ssize_t bytes_read;
    char ch;
    int transformari, suma=0;
    while((bytes_read=read(STDIN_FILENO, &ch, sizeof(char))))
    {
        if(ch=='\n')
        {
            break;
        }
        suma+=(int)(ch);
    }
    if(bytes_read==-1)
    {
        perror("Eroare la citire");
        exit(1);
    }
    if(-1==read(STDIN_FILENO, &transformari, sizeof(int)))
    {
        perror("Eroare la citire");
        exit(1);
    }
    *((int*)(shm_map+2*sizeof(int)))=suma-transformari;
    *((int*)(shm_map))=1;
}

int main()
{
    if(-1==mkfifo("myfifo", 0666))
    {
        if(errno!=EEXIST)
        {
            perror("Eroare la crearea fifo-ului in supervisor");
            exit(1);
        }
    }
    int fifo_fd=open("myfifo", O_WRONLY);
    if(-1==fifo_fd)
    {
        perror("Eroare la deschiderea capatului de citire al fifo-ului");
        exit(1);
    }
    
    char* shm_name="shm_open";
    int shm_fd=shm_open(shm_name, O_CREAT|O_RDWR, 0666);
    if(-1==shm_fd)
    {
        perror("Eroare la crearea obiectului de memorie partajata in supervisor");
        exit(1);
    }
    if(-1==ftruncate(shm_fd, 4096))
    {
        perror("Eroare la functia ftruncate");
        exit(1);
    }
    void* shm_map=mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(shm_map==MAP_FAILED)
    {
        perror("Eroare la mapare");
        exit(1);
    }
    while(*((char*)shm_map)!='r')
    {
        sleep(1);
    }
    sup2w1(shm_map, fifo_fd);
    w2tow1(shm_map);
    if(-1==munmap(shm_map, 4096))
    {
        perror("Eroare la demapare");
        exit(1);
    }
}
