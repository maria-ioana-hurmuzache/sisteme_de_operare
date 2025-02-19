#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

#define MAX_LINE_LENGTH 256

int main() 
{
    int channel_P1toP2[2], channel_P2toP1[2];
    pid_t pid_P1, pid_P2;

    //creez canale anonime
    if (pipe(channel_P1toP2) < 0 || pipe(channel_P2toP1) < 0) 
    {
        perror("Eroare la crearea canalelor anonime");
        exit(EXIT_FAILURE);
    }

    //scriu un octet in canalul P2toP1 pt a incepe comunicarea intre procese
    if (write(channel_P2toP1[1], "x", 1) < 0) 
    {
        perror("Eroare la scrierea în canal");
        exit(EXIT_FAILURE);
    }

    //creez procesul P1
    pid_P1 = fork();
    if (pid_P1 < 0) 
    {
        perror("Eroare la crearea procesului P1");
        exit(EXIT_FAILURE);
    } 
    else if (pid_P1 == 0) 
    {
        //proces P1
        //inchid capetele neutilizate ale canalelor
        close(channel_P2toP1[1]);
        close(channel_P1toP2[0]);

        FILE *file_nume = fopen("nume.txt", "r");
        if (!file_nume) 
        {
            perror("Eroare la deschiderea fișierului nume.txt");
            exit(EXIT_FAILURE);
        }

        char line[MAX_LINE_LENGTH];
        while (fgets(line, MAX_LINE_LENGTH, file_nume) != NULL) 
        {
            //astept semnal de la P2
            char signal;
            read(channel_P1toP2[0], &signal, 1);

            //elimin newline-ul de la sf liniei de nume
            size_t len_nume = strlen(line);
            if (line[len_nume - 1] == '\n') 
                line[len_nume - 1] = '\0';

            //scriu numele in fisier
            FILE *file_persoane = fopen("persoane.txt", "a");
            if (!file_persoane) 
            {
                perror("Eroare la deschiderea fișierului persoane.txt");
                fclose(file_nume);
                exit(EXIT_FAILURE);
            }
            fprintf(file_persoane, "%s ", line);
            fclose(file_persoane);

            //trimit semnal catre P2
            write(channel_P2toP1[1], "x", 1);
        }

        //inchid fisierele si canalele
        fclose(file_nume);
        close(channel_P2toP1[0]);
        close(channel_P1toP2[1]);

        exit(EXIT_SUCCESS);
    }

    //proces P2
    pid_P2 = fork();
    if (pid_P2 < 0) 
    {
        perror("Eroare la crearea procesului P2");
        exit(EXIT_FAILURE);
    } 
    else if (pid_P2 == 0) 
    {
        //proces P2
        //inchid capetele neutilizate ale canalelor
        close(channel_P1toP2[1]);
        close(channel_P2toP1[0]);

        FILE *file_prenume = fopen("prenume.txt", "r");
        if (!file_prenume) 
        {
            perror("Eroare la deschiderea fișierului prenume.txt");
            exit(EXIT_FAILURE);
        }

        char line[MAX_LINE_LENGTH];
        while (1) 
        {
            //astept semnal de la P1
            char signal;
            read(channel_P2toP1[0], &signal, 1);

            //citesc prenumele
            if (fgets(line, MAX_LINE_LENGTH, file_prenume) == NULL) 
                break; //ies din bucla daca ajung la EOF

            //elimin newline-ul de la sf liniei de prenume
            size_t len_prenume = strlen(line);
            if (line[len_prenume - 1] == '\n') 
                line[len_prenume - 1] = '\0';

            //scriu prenumele in fisier
            FILE *file_persoane = fopen("persoane.txt", "a");
            if (!file_persoane) 
            {
                perror("Eroare la deschiderea fișierului persoane.txt");
                fclose(file_prenume);
                exit(EXIT_FAILURE);
            }
            fprintf(file_persoane, "%s\n", line);
            fclose(file_persoane);

            //trimit semnal catre P1
            write(channel_P1toP2[1], "x", 1);
        }

        //inchid fisierele si canalele
        fclose(file_prenume);
        close(channel_P1toP2[0]);
        close(channel_P2toP1[1]);

        exit(EXIT_SUCCESS);
    }

    //inchid capetele neutilizate ale canalelor in procesul parinte
    close(channel_P1toP2[0]);
    close(channel_P2toP1[0]);
    close(channel_P1toP2[1]);
    close(channel_P2toP1[1]);

    //astept terminarea proceselor fii
    waitpid(pid_P1, NULL, 0);
    waitpid(pid_P2, NULL, 0);

    printf("Procesele P1 și P2 au terminat.\n");

    return 0;
}
