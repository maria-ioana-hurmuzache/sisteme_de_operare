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

void prelucrare_fisier(int fd, int write_fd)
{
    char ch;
    ssize_t bytes_read;
    while((bytes_read=read(fd, &ch, sizeof(char)))>0)
    {
        if((ch>='a' && ch<='z') || (ch>='A' && ch<='Z'))
        {
            //printf("%c\n", ch);
            if(-1==(write(write_fd, &ch, sizeof(char))))
            {
                perror("Eroare la scrierea in canalul anonim");
                close(fd);
                close(write_fd);
                exit(1);
            }
        }
    }
    if(-1==bytes_read)
    {
        perror("Eroare la citirea din fisier");
        close(fd);
        close(write_fd);
        exit(1);
    }
    if(-1==close(fd))
    {
        perror("Eroare la inchiderea fisierului");
        close(write_fd);
        exit(1);
    }
    if(-1==close(write_fd))
    {
        perror("Eroare la inchiderea capatului de scriere al canalului anonim");
        exit(1);
    }
}

void prelucrare_shm(int* shm_map)
{   
    int vocale=shm_map[0];
    int consoane=shm_map[1];
    
    printf("Numarul de vocale: %d\nNumarul de consoane: %d\n", vocale, consoane);
    
    if(-1==munmap(shm_map, 3*sizeof(int)))
    {
        perror("Eroare la demapare");
        exit(1);
    }
}

int main(int argc, char*argv[])
{
    if(argc!=2)
    {
        fprintf(stderr, "Nu ati introdus calea fisierului\n");
        exit(1);
    }
    
    int fd=open(argv[1], O_RDONLY);
    if(-1==fd)
    {
        perror("Eroare la deschiderea fisierului");
        exit(1);
    }
    
    //crearea maparii
    const char* shm_name="comunicare_decriptare";
    int shm_fd=shm_open(shm_name, O_RDWR, 0666);
    if(-1==shm_fd)
    {
        perror("Eroare la crearea/deschiderea memoriei partajate");
        exit(1);
    }
    if(-1==ftruncate(shm_fd, 3*sizeof(int)))
    {
        perror("Eroare la functia ftruncate");
        exit(1);
    }
    int* shm_map=mmap(NULL, 3*sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(shm_map==MAP_FAILED)
    {
        perror("Eroare la mapare");
        exit(1);
    }
    shm_map[2]=0;
    //crearea pipe-ului de comunicare intre supervisor si worker1
    int pipe_sup2w1[2];
    if(-1==pipe(pipe_sup2w1))
    {
        perror("Eroare la crearea canalului anonim");
        exit(1);
    }
    
    //crearea procesului fiu
    pid_t pid_w1;
    if(-1==(pid_w1=fork()))
    {
        perror("Eroare la crearea procesului fiu(fork)");
        exit(1);
    }
    if(pid_w1==0)
    {
        if(-1==close(pipe_sup2w1[1]))
        {
            perror("Eroare la inchiderea capatului de scriere al canalului anonim");
            exit(1);
        }
        //redirectarea capatului de citire
        if(-1==dup2(pipe_sup2w1[0], STDIN_FILENO))
        {
            perror("Eroare la redirectarea intrarii standard al canalului anonim");
            exit(1);
        }
        //apelul exec
        execl("../slaves/worker1", "../slaves/worker1", NULL);
        
        fprintf(stderr, "Eroare la apelul functiei execl\n");
        exit(1);
    }
    //procesul parinte
    if(-1==close(pipe_sup2w1[0]))
    {
        perror("Eroare la inchiderea capatului de citire al canalului anonim");
        exit(1);
    }
    //prelucrarea fisierului primit ca argument
    prelucrare_fisier(fd, pipe_sup2w1[1]);
    //asteptam terminarea procesului fiu
    wait(NULL);
    //prelucrarea datelor din memoria partajata
    while(shm_map[2]==0)
    {
        sleep(1);
    }
    prelucrare_shm(shm_map);
    if(-1==shm_unlink(shm_name))
    {
        perror("Eroare la unlink");
        exit(1);
    }
    
    return 0;
    
}
