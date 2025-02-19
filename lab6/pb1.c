#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#define BUFFER_SIZE 4096

void transformare(char oct, char binar[3])
{
	int val=oct-'0';
	binar[0]=(val & 4)?'1':'0';
	binar[1]=(val & 2)?'1':'0';
	binar[2]=(val & 1)?'1':'0';
}

int main (int argc, char *argv[])
{
	if (argc!=3)
	{
		fprintf(stderr, "ai nevoie de 2 fisiere\n");
		return EXIT_FAILURE;
	}

	if (strcmp(argv[1], argv[2])==0)
	{
		fprintf(stderr, "fisierele introduse sunt identice\n");
		return EXIT_FAILURE;
	}

	int input_fd = open (argv[1], O_RDONLY);
	if(input_fd==-1)
	{	
		perror("nu s-a putut deschide input file-ul");
		return EXIT_FAILURE;
	}

	int output_fd;
    char buffer[BUFFER_SIZE];
	ssize_t bytes_read;

	if (argc == 2)
	{
		output_fd = open(argv[1], O_RDWR, S_IRUSR|S_IWUSR);   
	}

	else
	{
		if (access(argv[2], F_OK)==0)
		{
			char raspuns;
			printf("Output-file-ul exista. Doresti sa il suprascrii? (y/n): ");
			scanf(" %c", &raspuns);
			if (raspuns != 'y' && raspuns != 'Y')
			{
                		close(input_fd);
                		return EXIT_SUCCESS;
            		}
		}
		output_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	}

	if (output_fd == -1)
	{
		perror("nu s-a putut deschide output file-ul");
		close(input_fd);
		return EXIT_FAILURE;
	}

	while ((bytes_read = read(input_fd, buffer, BUFFER_SIZE)) > 0)
	{
		for (ssize_t i=0; i<bytes_read; i++)
		{
			if (buffer[i]>='0' && buffer[i]<='7')
			{
				char binar[3];
				transformare(buffer[i], binar);
				write(output_fd, binar, 3);
			}
			else
				write(output_fd, &buffer[i], 1);
		}
	}
	if (bytes_read==-1)
	{
		perror("eroare la input file");
		close(input_fd);
		close(output_fd);
		return EXIT_FAILURE;
	}

	close(input_fd);
    close(output_fd);

	return EXIT_SUCCESS;

}
