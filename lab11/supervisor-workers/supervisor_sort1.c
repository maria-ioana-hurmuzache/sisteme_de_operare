#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define SHM_NAME_SIZE 256
#define PAGE_SIZE sysconf(_SC_PAGESIZE)

void error_exit(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void create_shared_memory(const char *shm_name, size_t size)
{
    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
    {
        error_exit("Failed to create shared memory");
    }
    if (ftruncate(shm_fd, size) == -1)
    {
        error_exit("Failed to set size of shared memory");
    }
    close(shm_fd);
}

void write_shared_memory(const char *shm_name, int offset, int length, const char *file_name)
{
    int shm_fd = shm_open(shm_name, O_RDWR, 0666);
    if (shm_fd == -1)
    {
        error_exit("Failed to open shared memory");
    }

    void *shm_ptr = mmap(NULL, sizeof(int) * 2 + strlen(file_name) + 1, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED)
    {
        error_exit("Failed to mmap shared memory");
    }

    int *int_ptr = (int *)shm_ptr;
    int_ptr[0] = offset;
    int_ptr[1] = length;
    char *file_name_ptr = (char *)(int_ptr + 2);
    strcpy(file_name_ptr, file_name);

    munmap(shm_ptr, sizeof(int) * 2 + strlen(file_name) + 1);
    close(shm_fd);
}

int count_lines(const char *content, size_t size)
{
    int lines = 0;
    for (size_t i = 0; i < size; i++)
    {
        if (content[i] == '\n')
        {
            lines++;
        }
    }
    return lines;
}

void get_file_chunks(const char *content, size_t size, int num_workers, int *offsets, int *lengths)
{
    int total_lines = count_lines(content, size);
    int lines_per_chunk = total_lines / num_workers;
    int extra_lines = total_lines % num_workers;

    int current_offset = 0;
    int current_line = 0;

    for (int i = 0; i < num_workers; i++)
    {
        int lines_in_chunk = lines_per_chunk + (i < extra_lines ? 1 : 0);
        int chunk_size = 0;

        while (lines_in_chunk > 0 && current_offset + chunk_size < size)
        {
            if (content[current_offset + chunk_size] == '\n')
            {
                lines_in_chunk--;
            }
            chunk_size++;
        }

        offsets[i] = current_offset;
        lengths[i] = chunk_size;
        current_offset += chunk_size;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <num_workers> <input_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int num_workers = atoi(argv[1]);
    const char *input_file = argv[2];

    int input_fd = open(input_file, O_RDONLY);
    if (input_fd == -1)
    {
        error_exit("Failed to open input file");
    }

    struct stat file_stat;
    if (fstat(input_fd, &file_stat) == -1)
    {
        error_exit("Failed to get file stats");
    }

    size_t file_size = file_stat.st_size;
    char *file_content = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, input_fd, 0);
    if (file_content == MAP_FAILED)
    {
        error_exit("Failed to mmap input file");
    }
    close(input_fd);

    int *offsets = malloc(num_workers * sizeof(int));
    int *lengths = malloc(num_workers * sizeof(int));
    if (offsets == NULL || lengths == NULL)
    {
        error_exit("Failed to allocate memory for offsets or lengths");
    }

    get_file_chunks(file_content, file_size, num_workers, offsets, lengths);

    char shm_names[num_workers][SHM_NAME_SIZE];
    
    for (int i = 0; i < num_workers; i++)
    {
        snprintf(shm_names[i], SHM_NAME_SIZE, "/shm_worker_%d", i);
        create_shared_memory(shm_names[i], lengths[i] + sizeof(int) * 2 + strlen(input_file) + 1);
        write_shared_memory(shm_names[i], offsets[i], lengths[i], input_file);
    }

    free(offsets);
    free(lengths);

    for (int i = 0; i < num_workers; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            execlp("./worker_sort1", "worker_sort1", shm_names[i], (char *)NULL);
            error_exit("Failed to exec worker_sort1");
        }
        else if (pid == -1)
        {
            error_exit("Failed to fork");
        }
    }

    for (int i = 0; i < num_workers; i++)
    {
        int status;
        wait(&status);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
        {
            fprintf(stderr, "Worker %d failed\n", i);
            exit(EXIT_FAILURE);
        }
    }
    
    for (int i = 0; i < num_workers; i++)
    {
        int shm_fd = shm_open(shm_names[i], O_RDONLY, 0666);
        if (shm_fd == -1)
        {
            error_exit("Failed to open shared memory");
        }

        struct stat st;
        if (fstat(shm_fd, &st) == -1)
        {
            error_exit("Failed to get shared memory stats");
        }

        size_t shm_size = st.st_size;
        void *shm_ptr = mmap(NULL, shm_size, PROT_READ, MAP_SHARED, shm_fd, 0);
        if (shm_ptr == MAP_FAILED)
        {
            error_exit("Failed to mmap shared memory");
        }

        printf("%s", (char *)shm_ptr);

        munmap(shm_ptr, shm_size);
        close(shm_fd);
        shm_unlink(shm_names[i]);
    }

    munmap(file_content, file_size);

    return 0;
}
