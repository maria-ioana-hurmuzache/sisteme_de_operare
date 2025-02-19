#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>

#define BUFFER_SIZE 4096

int main (int argc, char*argv[])
{
	if (argc!=2)
	{
		fprintf(stderr, "nr argumente necorespunzator\n");
		return 2;
	}

	if (!(access(argv[1], R_OK)==0))
	{
		if (errno==EACCES)
		{
			fprintf(stderr, "Fisierul nu poate fi citit!\n");
		}
		else
		{
			fprintf(stderr, "Eroare la verificarea permisiunii de citire!\n");
		}
		return 3;
	}
	int fd;
	fd=open(argv[1], O_RDONLY);
	if (fd==-1)
	{
		fprintf(stderr, "Eroare la deschiderea fisierului!\n");
		return 4;
	}

	char buffer[BUFFER_SIZE];
	ssize_t bytes_read;
	int nr_caractere_mari=0;
	int nr_caractere_mici=0;

	while((bytes_read=read(fd, buffer, BUFFER_SIZE))>0)
	{
		for (ssize_t i=0; i<bytes_read; i++)
		{
			if (buffer[i]>='A' && buffer[i]<='Z')
				nr_caractere_mari++;
			else if (buffer[i]>='a' && buffer[i]<='z')
				nr_caractere_mici++;
		}

	}
	if (bytes_read==-1)
	{
		fprintf(stderr, "Eroare la citirea fisierului!\n");
		close(fd);
		return 4;
	}

	close(fd);
	printf("%s:%d:%d\n",argv[1],nr_caractere_mari,nr_caractere_mici);
	return 0;
}
