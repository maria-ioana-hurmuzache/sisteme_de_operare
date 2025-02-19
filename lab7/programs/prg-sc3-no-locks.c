
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define RECORD_SIZE 10

void update_stock( const char* filename, int product_code, float quantity) {
   
    FILE *file = fopen(filename, "rb+");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    int found = 0;
    while (!feof(file)) {
        int code;
        float stock;
        fread(&code, sizeof(int), 1, file);
        fread(&stock, sizeof(float), 1, file);

        if (code == product_code) {
            found = 1;
            if (quantity < 0 && stock + quantity < 0) {
                printf("PID %d: Error: Insufficient stock for product code %d\n", getpid(), product_code);
                break;
            } else {
                stock += quantity;
                fseek(file, -sizeof(float), SEEK_CUR);
                fwrite(&stock, sizeof(float), 1, file);
                printf("PID %d: Stock updated for product code %d\n", getpid(), product_code);
                break;
            }
        }
    }

    if (!found && quantity < 0) {
        printf("PID %d: Error: Product code %d not found\n", getpid(), product_code);
    }

    fclose(file);
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

