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

#define BUFFER_SIZE 4096

int main(int argc, char* argv[])
{
	if (argc!=2)
	{
		fprintf(stderr, "Programul primeste un singur argument!\n");
		return 2;
	}

	struct stat st;
	if (stat(argv[1], &st)==-1)
	{
		fprintf(stderr, "Eroare la functia stat! Fisierul nu exista sau nu poate fi accesat!\n");// verificarea asta putea fi facuta cu acces(argv[1], F_OK)!=0
		return 2;
	}

	if ((st.st_mode & S_IFREG)==0)
	{
		fprintf(stderr, "Argumentul introdus nu este fisier!\n");
		return 2;
	}

	if (!(access(argv[1], R_OK)==0)) //verific permisiunea de citire; puteam si cu stat:if(st.st_mode & S_IRUSR)!=0 -> utilizatorul curent are permisiunea de citire
	{
		if (errno==EACCES)
		{
			fprintf(stderr, "Fisierul nu poate fi citit!\n");
		}
		else
		{
			fprintf(stderr, "Eroare la verificarea permisiunii de citire!\n");
		}
		return 2;
	}

	int fd;
	fd=open(argv[1], O_RDONLY);
	if (fd==-1)
	{
		fprintf(stderr, "Eroare la deschiderea fisierului!\n");
		return 3;
	}

	if(st.st_size==0)
	{
		printf("%s:not alphanumeric\n", argv[1]);
		close(fd);
		return 0;
	}

	char buffer[BUFFER_SIZE];
	ssize_t bytes_read;
	int isAlphaNumeric=1;

	while((bytes_read=read(fd, buffer, BUFFER_SIZE))>0 && isAlphaNumeric==1)
	{
		for (ssize_t i=0; i<bytes_read; i++)
		{
			if((!(isalnum(buffer[i]))) && buffer[i]!='\n')
			{
				isAlphaNumeric=0;
                		//printf("Caracterul gresit:%c",buffer[i]);
				break;
			}
		}

	}

	if (bytes_read==-1)
	{
		fprintf(stderr, "Eroare la citirea fisierului!\n");
		close(fd);
		return 3;
	}

	close(fd);

	if(isAlphaNumeric==1)
	{
		printf("%s:alphanumeric\n", argv[1]);
		return 0;
	}
	else
	{
		printf("%s:not alphanumeric\n", argv[1]);
		return 0;
	}
}
