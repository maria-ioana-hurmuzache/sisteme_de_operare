#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <string.h>

void transformare(char oct, char binar[3]) {
    int val = oct - '0';
    binar[0] = (val & 4) ? '1' : '0';
    binar[1] = (val & 2) ? '1' : '0';
    binar[2] = (val & 1) ? '1' : '0';
}

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3)
    {
        fprintf(stderr, "Ai nevoie de 1 sau 2 fisiere.\n");
        return EXIT_FAILURE;
    }

    char *input_file = argv[1];
    char *output_file;
    int same=0;

    if (argc == 2 || strcmp(argv[1], argv[2]) == 0)
    {
        // Dacă există un singur argument sau sunt identice, tratăm ca fișier temporar
        output_file = "temp_output.txt"; // Un nume temporar pentru fișierul de ieșire
        same=1;
    }
    else
    {
        output_file = argv[2];
    }

    // Deschide fișierul de intrare
    int input_fd = open(input_file, O_RDONLY);
    if (input_fd == -1)
    {
        perror("Nu s-a putut deschide input file-ul.");
        return EXIT_FAILURE;
    }

    // Obține dimensiunea fișierului de intrare
    struct stat st;
    if (fstat(input_fd, &st) == -1)
    {
        perror("Eroare la fstat.");
        close(input_fd);
        return EXIT_FAILURE;
    }

    size_t input_size = st.st_size;

    // Mapare fișier de intrare în memorie
    char *input_data = mmap(NULL, input_size, PROT_READ, MAP_SHARED, input_fd, 0);
    if (input_data == MAP_FAILED)
    {
        perror("Eroare la mapare.");
        close(input_fd);
        return EXIT_FAILURE;
    }
    
    if((access(output_file, F_OK))==0 || same==1)
    {
        if(same==1)
        {
            printf("Fișierul %s există deja. Dorești să-l suprascrii? (y/n): \n", input_file);
        }
        else
        {
            printf("Fișierul %s există deja. Dorești să-l suprascrii? (y/n): \n", output_file);
        }
        
        char c;
        if(read(STDIN_FILENO, &c, 1)==-1)
        {
            perror("Citire caracter");
            close(input_fd);
            munmap(input_data, input_size);
            return EXIT_FAILURE;
        }
        if(c=='n' || c=='N')
        {
            close(input_fd);
            munmap(input_data, input_size);
            return 0;
        }
    }
    
    // Deschide fișierul de ieșire și alocă spațiu
    int output_fd = open(output_file, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (output_fd == -1)
    {
        perror("Nu s-a putut deschide output file-ul.");
        munmap(input_data, input_size);
        close(input_fd);
        return EXIT_FAILURE;
    }

    // Estimează dimensiunea fișierului de ieșire
    size_t output_size = input_size * 3;
    if (ftruncate(output_fd, output_size) == -1)
    {
        perror("Eroare la ftruncate.");
        munmap(input_data, input_size);
        close(input_fd);
        close(output_fd);
        return EXIT_FAILURE;
    }

    // Mapare fișier de ieșire în memorie
    char *output_data = mmap(NULL, output_size, PROT_WRITE, MAP_SHARED, output_fd, 0);
    if (output_data == MAP_FAILED)
    {
        perror("Eroare la mapare pentru fișierul de ieșire.");
        munmap(input_data, input_size);
        close(input_fd);
        close(output_fd);
        return EXIT_FAILURE;
    }

    // Copiază și convertește datele
    size_t pos = 0;
    for (size_t i = 0; i < input_size; i++)
    {
        if (input_data[i] >= '0' && input_data[i] <= '7')
        {
            char binar[3];
            transformare(input_data[i], binar);
            output_data[pos++] = binar[0];
            output_data[pos++] = binar[1];
            output_data[pos++] = binar[2];
        }
        else
        {
            output_data[pos++] = input_data[i];
        }
    }
    
    if (ftruncate(output_fd, pos) == -1)
    {
        perror("Eroare la ajustarea dimensiunii fișierului.");
        return EXIT_FAILURE;
    }

    // Dezmapare și închidere fișiere
    munmap(input_data, input_size);
    munmap(output_data, output_size);
    close(input_fd);
    close(output_fd);

    // Dacă am folosit un fișier temporar, înlocuiește fișierul original cu acesta
    if (same==1)
    {
        if (rename(output_file, input_file) == -1)
        {
            perror("Eroare la înlocuirea fișierului original.");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

