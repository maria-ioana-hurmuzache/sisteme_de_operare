#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFFER_SIZE 4096

void print_usage(const char *program_name) {
    fprintf(stderr, "Usage: %s [-c] [-w N] [file1] [file2] ...\n", program_name);
}

int main(int argc, char *argv[]) {
    int opt;
    int print_count = 0; // opțiunea -c pentru număr de apariții
    int width = -1; // dacă nu este specificat, se compară întreaga linie

    // Procesăm opțiunile din linia de comandă
    while ((opt = getopt(argc, argv, "cw:")) != -1) {
        switch (opt) {
            case 'c':
                print_count = 1; // afișează contorul pentru liniile unice
                break;
            case 'w':
                width = atoi(optarg);
                if (width <= 0) {
                    fprintf(stderr, "Invalid width specified for -w option\n");
                    return EXIT_FAILURE;
                }
                break;
            default:
                print_usage(argv[0]);
                return EXIT_FAILURE;
        }
    }

    // Verificăm dacă avem cel puțin un fișier de procesat
    if (optind >= argc) {
        fprintf(stderr, "No files specified.\n");
        return EXIT_FAILURE;
    }

    // Procesăm fișierele specificate
    for (int i = optind; i < argc; i++) {
        int fd = open(argv[i], O_RDONLY);
        if (fd < 0) {
            perror("Nu am putut deschide fișierul");
            return EXIT_FAILURE;
        }

        char *buffer = (char *)malloc(BUFFER_SIZE);
        if (!buffer) {
            perror("Memory allocation failed");
            close(fd);
            return EXIT_FAILURE;
        }

        char previous_line[BUFFER_SIZE] = "";
        int count = 0;
        ssize_t bytes_read;

        while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
            char *line = strtok(buffer, "\n");
            while (line != NULL) {
                // Dacă width este specificat, comparăm doar primele N caractere
                int cmp_len = (width != -1) ? width : strlen(line);

                // Dacă linia curentă este diferită de cea anterioară
                if (strncmp(previous_line, line, cmp_len) != 0)
                {
                    // Afișăm ultima linie unică înainte de a trece la următoarea
                    if (previous_line[0] != '\0') {
                        if (print_count)
                        {
                            printf("%d %s\n", count, previous_line);
                        }
                        else
                        {
                            printf("%s\n", previous_line);
                        }
                    }
                    // Resetăm contorul și actualizăm linia anterioară
                    count = 1;
                    strcpy(previous_line, line);
                }
                else
                {
                    // Dacă este aceeași linie, incrementăm contorul
                    count++;
                }

                line = strtok(NULL, "\n"); // continuăm cu linia următoare
            }
        }

        // Afișăm ultima linie după terminarea citirii fișierului
        if (previous_line[0] != '\0')
        {
            if (print_count)
            {
                printf("%d %s\n", count, previous_line);
            }
            else
            {
                printf("%s\n", previous_line);
            }
        }

        free(buffer); // eliberăm memoria
        close(fd); // închidem fișierul
    }

    return EXIT_SUCCESS;
}

