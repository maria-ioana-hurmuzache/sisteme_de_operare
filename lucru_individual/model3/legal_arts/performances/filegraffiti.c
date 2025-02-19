#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>

int main(int argc, char* argv[])
{
	if(argc!=3)
	{
		fprintf(stderr, "Numarul de argumente introduse nu corespunde!\n");
		return 2;
	}

	const char* fileName=argv[1];
	const char* cuvant=argv[2];

	struct stat fileInfo;
	if (stat(fileName, &fileInfo)==-1)
	{
		fprintf(stderr, "Eroare la functia stat\n");
		return 3;
	}

	size_t lungime_cuvant=strlen(cuvant);

	if(fileInfo.st_size<lungime_cuvant)
	{
		fprintf(stdout,"CANVAS TOO SMALL\n");
		return 0;
	}

	int fd=open(fileName, O_RDWR);
	if (fd==-1)
	{
		fprintf(stderr, "Eroare la deschiderea fisierului\n");
		return 4;
	}

	off_t offset=lseek(fd, -lungime_cuvant, SEEK_END);
	if (offset==-1)
	{
		fprintf(stderr, "Eroare la obtinerea offset-ului\n");
		return 5;
	}

	if(write(fd, cuvant, lungime_cuvant)==-1)
	{
		fprintf(stderr, "Eroare la scrierea in fisier\n");
		return 6;
	}
	
	if (close(fd)==-1)
	{
		fprintf(stderr, "Eroare la inchiderea fisierului\n");
		return 7;
	}

	fprintf(stdout,"FILE VANDALIZED: %s\n", fileName);

	return 0;
}
