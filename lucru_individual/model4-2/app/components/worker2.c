#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

void citire_fifo(int fifo_fd, char*shm_ptr)
{
    char ch;
    int frecv;
    ssize_t bytes_read;
    char vocale[]="aeiou";
    int f_vocale[5]={0};
    char consoane[]="bcdfghjklmnpqrstvwxyz";
    int f_consoane[21]={0};
    int voc_max=0, cons_min=4096;
    char voc, cons;
    
    while((bytes_read=read(fifo_fd, &ch, sizeof(char)))>0)
    {
        if(-1==(read(fifo_fd, &frecv, sizeof(int))))
        {
            perror("Eroare la citire din fifo");
            exit(1);
        }
        ch=tolower(ch);
        char*pos;
        if((pos=strchr(vocale, ch))!=NULL)
        {
            f_vocale[pos-vocale]+=frecv;
        }
        else if((pos=strchr(consoane, ch))!=NULL)
        {   
            f_consoane[pos-consoane]+=frecv;
        }
    }
    if(-1==bytes_read)
    {
        perror("Eroare la read");
        exit(1);
    }
    for (size_t i=0; i<strlen(vocale); i++)
    {
        if(f_vocale[i]>voc_max)
        {
            voc=vocale[i];
            voc_max=f_vocale[i];
        }
    }
    for (size_t i=0; i<strlen(consoane); i++)
    {
        if(f_consoane[i]<cons_min && f_consoane[i]!=0)
        {
            cons_min=f_consoane[i];
            cons=consoane[i];
        }
    }

    *(char*)shm_ptr=voc;
    *(int*)(shm_ptr+sizeof(char))=voc_max;
    *(char*)(shm_ptr+sizeof(char)+sizeof(int))=cons;
    *(int*)(shm_ptr+2*sizeof(char)+sizeof(int))=cons_min;
    *(int*)(shm_ptr+2*(sizeof(char)+sizeof(int)))=1;
    if(-1==msync(shm_ptr, 2*(sizeof(char)+sizeof(int))+sizeof(char),MS_SYNC))
    {
        perror("msync");
        exit(1);
    }
    
}

int main(int argc, char*argv[])
{
    if(mkfifo("fifo1_2",0666)==-1)
    {
        if(errno!=EEXIST)
        {
            perror("Eroare la crearea fifo-ului");
            exit(1);
        }
    }
    
    int fifo_fd=open("fifo1_2", O_RDONLY);
    if(fifo_fd==-1)
    {
        perror("Eroare la deschiderea fifo-ului");
        exit(1);
    }
    
    
    int shm_fd;
    void* shm_ptr;
    
    shm_fd=shm_open("myshm", O_CREAT | O_RDWR, 0666);
    if(shm_fd==-1)
    {
        perror("Eroare la crearea memoriei partajate");
        exit(1);
    }
    
    if (ftruncate(shm_fd, 2*sizeof(char)+3*sizeof(int))==-1)
    {
        perror("Eroare la trunchierea memoriei partajate");
        exit(1);
    }
    
    shm_ptr=mmap(NULL, 2*sizeof(char)+3*sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr==MAP_FAILED)
    {
        perror("Eroare la mapare");
        exit(1);
    }
    
    citire_fifo(fifo_fd, shm_ptr);
    
    if(-1==(munmap(shm_ptr, 2*sizeof(char)+3*sizeof(int))))
    {
        perror("Eroare la eliminarea maparii memoriei partajate");
        exit(1);
    }
    return 0;
}
