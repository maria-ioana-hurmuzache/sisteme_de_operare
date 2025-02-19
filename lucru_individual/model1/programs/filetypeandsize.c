#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUF_SIZE 4096

int main( int argc, char *argv[])
{
	if(argc!=2)
	{
		fprintf(stderr, "Programul are nevoie de un singur argument!");
		return 2;
	}

	struct stat st;
	if(stat(argv[1],&st)==-1)
	{
		fprintf(stderr, "Eroare la stat pentru %s .\n", argv[1]);
		perror("Cauza este: ");
		return 3;
	}

	switch(st.st_mode & S_IFMT)
	{
		case S_IFDIR : printf("Directory\n"); break;
		case S_IFREG : printf("Regular\n"); break;
		case S_IFLNK : printf("Link\n"); break;
		case S_IFIFO : printf("FIFO\n"); break;
		case S_IFBLK : printf("Block device\n"); break;
		case S_IFSOCK : printf("Socket\n"); break;
		case S_IFCHR : printf("Character Device"); break;
		default : printf("Unknown");
	}

	printf("%s : %ld\n", argv[1], st.st_size);

	if ((st.st_mode &S_IFMT)==S_IFDIR)
		return 1;
	else
		return 0;
}
