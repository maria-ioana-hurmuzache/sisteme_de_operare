#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int my_system(const char *command) {
    if (command == NULL)
    {
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork failed");
        return -1;
    }

    if (pid == 0)
    {
        // Procesul copil
        execl("/bin/sh", "sh", "-c", command, (char *)NULL);
        // Dacă exec eșuează
        perror("execl failed");
        exit(EXIT_FAILURE);
    }
    else
    {
        // Procesul părinte
        int status;
        if (waitpid(pid, &status, 0) == -1)
        {
            perror("waitpid failed");
            return -1;
        }
        if (WIFEXITED(status))
        {
            return WEXITSTATUS(status);
        } 
        else
        {
            return -1;
        }
    }
}

int main(int argc, char* argv[])
{
    const char *command = argv[1];

    int result = my_system(command);
    if (result == -1)
    {
        printf("Command execution failed\n");
    }
    else
    {
        printf("Command executed with return status: %d\n", result);
    }

    return 0;
}

