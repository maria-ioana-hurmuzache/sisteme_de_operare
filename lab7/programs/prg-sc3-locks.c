// programs/prg-sc3-locks.c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define RECORD_SIZE (sizeof(int) + sizeof(float))

void update_stock(const char *filename, int product_code, float quantity) {
    int file = open(filename, O_RDWR);
    if (file == -1) {
        fprintf(stderr, "Error opening file: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = (product_code - 1) * RECORD_SIZE;
    lock.l_len = RECORD_SIZE;

    if (fcntl(file, F_SETLKW, &lock) == -1) {
        fprintf(stderr, "Error locking file: %s\n", strerror(errno));
        close(file);
        exit(EXIT_FAILURE);
    }

    int found = 0;
    lseek(file, lock.l_start, SEEK_SET);
    while (read(file, &found, sizeof(int)) == sizeof(int)) {
        float stock;
        read(file, &stock, sizeof(float));

        if (found == product_code) {
            found = 1;
            if (quantity < 0 && stock + quantity < 0) {
                printf("PID %d: Error: Insufficient stock for product code %d\n", getpid(), product_code);
                break;
            } else {
                stock += quantity;
                lseek(file, -sizeof(float), SEEK_CUR);
                write(file, &stock, sizeof(float));
                printf("PID %d: Stock updated for product code %d\n", getpid(), product_code);
                break;
            }
        }
    }

    if (!found && quantity < 0) {
        printf("PID %d: Error: Product code %d not found\n", getpid(), product_code);
    }

    lock.l_type = F_UNLCK;
    fcntl(file, F_SETLK, &lock);
    close(file);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <database_file> <instruction_file> <pid>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *database_file = argv[1];
    const char *instruction_file = argv[2];
    int pid = atoi(argv[3]);

    FILE *instructions = fopen(instruction_file, "r");
    if (instructions == NULL) {
        fprintf(stderr, "Error opening instruction file: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    int product_code;
    float quantity;
    while (fscanf(instructions, "%d %f", &product_code, &quantity) != EOF) {
        update_stock(database_file, product_code, quantity);
        sleep(1); // Introduce a short delay between updates
    }

    fclose(instructions);
    return EXIT_SUCCESS;
}

