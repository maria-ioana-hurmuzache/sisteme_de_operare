/*Să se scrie un program în limbajul C care să permită mai multor utilizatori să acceseze simultan un fișier de resurse partajate. Fiecare utilizator trebuie să poată să citească și să scrie în fișier, însă trebuie să se asigure că nu există conflicte între accesuri. Utilizarea lacatelor din API-ul POSIX este necesară pentru a implementa această funcționalitate, astfel încât să fie evitate situațiile de citire sau scriere concurentă care ar putea corupe datele.*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define FILE_PATH "shared_file.txt"
#define BUFFER_SIZE 256

void read_from_file(int fd) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    // Blochează fișierul pentru citire
    if (flock(fd, LOCK_SH) == -1) {
        perror("Eroare la blocarea fișierului pentru citire");
        exit(EXIT_FAILURE);
    }

    // Citirea din fișier
    printf("Conținutul fișierului:\n");
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        write(STDOUT_FILENO, buffer, bytes_read);
    }
    if (bytes_read == -1) {
        perror("Eroare la citirea din fișier");
        exit(EXIT_FAILURE);
    }

    // Deblochează fișierul
    if (flock(fd, LOCK_UN) == -1) {
        perror("Eroare la deblocarea fișierului pentru citire");
        exit(EXIT_FAILURE);
    }
}

void write_to_file(int fd, const char *message) {
    // Blochează fișierul pentru scriere
    if (flock(fd, LOCK_EX) == -1) {
        perror("Eroare la blocarea fișierului pentru scriere");
        exit(EXIT_FAILURE);
    }

    // Scrierea în fișier
    if (write(fd, message, strlen(message)) == -1) {
        perror("Eroare la scrierea în fișier");
        exit(EXIT_FAILURE);
    }

    // Deblochează fișierul
    if (flock(fd, LOCK_UN) == -1) {
        perror("Eroare la deblocarea fișierului pentru scriere");
        exit(EXIT_FAILURE);
    }
}

int main() {
    int fd;

    // Deschide fișierul
    fd = open(FILE_PATH, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("Eroare la deschiderea fișierului");
        exit(EXIT_FAILURE);
    }

    // Exemplu de utilizare: Citire și scriere în fișier
    read_from_file(fd);
    write_to_file(fd, "\nMesajul a fost modificat de către Utilizatorul 1.");
    read_from_file(fd);
    write_to_file(fd, "\nMesajul a fost modificat de către Utilizatorul 2.");

    // Închide fișierul
    if (close(fd) == -1) {
        perror("Eroare la închiderea fișierului");
        exit(EXIT_FAILURE);
    }

    return 0;
}

