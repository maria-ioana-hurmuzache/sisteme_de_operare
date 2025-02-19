#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define MAX_LINES 1000
#define PAGE_SIZE sysconf(_SC_PAGESIZE)

void error_exit(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <shm_name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *shm_name = argv[1];

    int shm_fd = shm_open(shm_name, O_RDWR, 0666);
    if (shm_fd == -1)
    {
        error_exit("Failed to open shared memory");
    }

    struct stat st;
    if (fstat(shm_fd, &st) == -1)
    {
        close(shm_fd);
        error_exit("Failed to get shared memory stats");
    }

    size_t shm_size = st.st_size;
    void *shm_ptr = mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED)
    {
        close(shm_fd);
        error_exit("Failed to mmap shared memory");
    }

    close(shm_fd);

    int *int_ptr = (int *)shm_ptr;
    int offset = int_ptr[0];
    int length = int_ptr[1];
    char *file_name = (char *)(int_ptr + 2);

    off_t aligned_offset = offset & ~(PAGE_SIZE - 1);
    size_t offset_diff = offset - aligned_offset;
    size_t map_size = length + offset_diff;

    int input_fd = open(file_name, O_RDONLY);
    if (input_fd == -1)
    {
        error_exit("Failed to open input file");
    }

    char *file_content = mmap(NULL, map_size, PROT_READ, MAP_PRIVATE, input_fd, aligned_offset);
    if (file_content == MAP_FAILED)
    {
        close(input_fd);
        error_exit("Failed to mmap input file portion");
    }

    close(input_fd);

    char *data_start = file_content + offset_diff;

    char **lines = malloc(MAX_LINES * sizeof(char *));
    int line_count = 0;

    char *content_copy = strndup(data_start, length);
    if (content_copy == NULL) {
        error_exit("Failed to allocate memory for content copy");
    }

    char *line = strtok(content_copy, "\n");
    while (line != NULL && line_count < MAX_LINES) {
        lines[line_count++] = strdup(line);
        line = strtok(NULL, "\n");
    }

    for (int i = 0; i < line_count - 1; i++)
    {
        for (int j = 0; j < line_count - i - 1; j++)
        {
            if (strcmp(lines[j], lines[j + 1]) < 0)
            {
                char *temp = lines[j];
                lines[j] = lines[j + 1];
                lines[j + 1] = temp;
            }
        }
    }

    char *output_ptr = (char *)shm_ptr;
    output_ptr[0] = '\0';

    for (int i = 0; i < line_count; i++)
    {
        strcat(output_ptr, lines[i]);
        strcat(output_ptr, "\n");
        free(lines[i]);
    }

    free(lines);
    free(content_copy);
    munmap(file_content, map_size);
    munmap(shm_ptr, shm_size);

    return 0;
}
