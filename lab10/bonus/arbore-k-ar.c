#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void creeaza_procese_recursiv(int nivel, int K, int numar_parinte, int numar_fiu, int N)
{
    pid_t pid;
    if(nivel!=1)
        printf("Procesul (%d,%d): PID=%d, PID parinte=%d\n", nivel, numar_fiu, getpid(), numar_parinte);

    if (nivel < N)
    {
        for (int i = 1; i <= K; ++i)
        {
            if ((pid = fork()) == -1)
            {
                perror("Eroare la fork");
                exit(EXIT_FAILURE);
            }
            else if (pid == 0)
            {
                // Proces fiu
                creeaza_procese_recursiv(nivel + 1, K, getppid(), i, N);
                exit(EXIT_SUCCESS);
            }
        }

        // Așteptarea terminării tuturor fiilor
        for (int i = 1; i <= K; ++i)
        {
            wait(NULL);
        }
    }
}

int main()
{
    int K, N;

    printf("Introduceti valoarea lui K (numarul de procese fiu pe fiecare nivel): ");
    scanf("%d", &K);

    printf("Introduceti valoarea lui N (numarul de nivele al arborelui): ");
    scanf("%d", &N);

    // Creează procesul rădăcină
    pid_t pid = getpid();
    printf("Procesul (1,1): PID=%d\n", pid);

    // Creează arborele recursiv
    creeaza_procese_recursiv(1, K, pid, 1, N);

    return 0;
}

