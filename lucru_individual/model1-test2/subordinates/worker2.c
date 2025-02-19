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

void worker2supervisor(int* shm_map)
{
    ssize_t bytes_read;
    int nr, par_max=0, impar_min=99999;
    while((bytes_read=read(STDIN_FILENO, &nr, sizeof(int))))
    {
        if(nr%2==0 && nr>par_max)
        {
            par_max=nr;
        }
        if(nr%2==1 && nr<impar_min)
        {
            impar_min=nr;
        }
    }
    if(bytes_read==-1)
    {
        perror("Eroare la read in worker2");
        exit(1);
    }
    shm_map[0]=par_max;
    shm_map[1]=impar_min;
    shm_map[2]=1;
    if(-1==msync(shm_map, 3*sizeof(int), MS_SYNC))
    {
        perror("Eroare la msync");
        exit(1);
    }
    if(-1==munmap(shm_map, 3*sizeof(int)))
    {
        perror("Eroare la demapare");
        exit(1);
    }
}

int main()
{
    //initializarea memoriei partajate
    const char* shm_name="myshm";
    int shm_fd=shm_open(shm_name, O_RDWR, 0666);
    if(-1==shm_fd)
    {
        perror("Eroare la crearea obiectului de memorie partajata in supervisor");
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
    
    worker2supervisor(shm_map);
}
