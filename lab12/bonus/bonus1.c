#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_CMDS 10
#define MAX_ARGS 10
#define BUF_SIZE 1024

void parse_command(char* input, char* commands[MAX_CMDS][MAX_ARGS]) 
{
    int cmd_count = 0; //contor pt nr de comenzi
    int arg_count = 0; //contor pt nr de argumente ale unei comenzi
    char* arg_start = NULL; //pointer care memoreaza inceputul unui argument
    int in_quote = 0; //flag pt a verifica daca suntem intr-un sir citat (fpt argumentele cu spatii)

    for (char* c = input; *c != '\0'; c++) 
    {
        if (*c == ' ' && !in_quote) //daca caracterul curent este un spatiu si nu suntem intr-un sirr citat
        {
            if (arg_start != NULL) //inseamna ca am inceput deja un argument
            {
                *c = '\0';
                commands[cmd_count][arg_count++] = arg_start;
                arg_start = NULL;
            }
        } 
        else if (*c == '"') 
        {
            if (!in_quote) 
            {
                in_quote = 1;
                arg_start = c + 1; //sarim peste " de inceput
            } 
            else 
            {
                in_quote = 0;
                *c = '\0'; //terminam argumentul
                commands[cmd_count][arg_count++] = arg_start;
                arg_start = NULL;
            }
        } 
        else if (*c == '|') 
        {
            if (in_quote) //daca suntem intr-un sir citat, ignoram '|'
                continue;
            if (arg_start != NULL) 
            {
                *c = '\0'; //terminam argumentul curent
                commands[cmd_count][arg_count++] = arg_start;
                arg_start = NULL;
            }
            commands[cmd_count][arg_count] = NULL;
            cmd_count++;
            arg_count = 0; //resetam arg_count la 0 pt a incepe contorizarea arg pt urm comanda 
        } 
        else 
        {
            if (arg_start == NULL)
                arg_start = c;
        }
    }
    if (arg_start != NULL) //dupa ce iesim din bucla, daca arg_start nu este NULL, inseamna ca mai avem un argument neterminat
        commands[cmd_count][arg_count++] = arg_start;
    commands[cmd_count][arg_count] = NULL; //pt a marca sf. listei de argumente pt ultima comanda
    commands[cmd_count + 1][0] = NULL; //pt a marca sf. listei de comenzi
}

int main() 
{
    char input[BUF_SIZE];
    char* commands[MAX_CMDS][MAX_ARGS];

    while (1)
    {
        printf("UNIX> ");
        if (fgets(input, BUF_SIZE, stdin) == NULL) 
            break;

        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') 
            input[len - 1] = '\0';

        parse_command(input, commands);

        int num_cmds = 0;
        while (commands[num_cmds][0] != NULL) 
            num_cmds++;

        int pipefds[2 * (num_cmds - 1)];
        for (int i = 0; i < num_cmds - 1; i++) 
        {
            if (pipe(pipefds + i * 2) == -1) 
            {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
        }

        for (int i = 0; i < num_cmds; i++) 
        {
            pid_t pid = fork();
            if (pid == 0) 
            {
                if (i != 0) //daca i nu e 0 (nu e prima comanda), aceasta comanda tb sa primeasca input din pipe-ul creat de comanda anterioara.
//dup2(pipefds[(i - 1) * 2], 0) redirectioneaza stdin (file descriptor 0) la file descriptor-ul de citire al pipe-ului creat pentru comanda anterioara (pipefds[(i - 1) * 2]).
                    dup2(pipefds[(i - 1) * 2], 0);
                if (i != num_cmds - 1) //Dacă i nu e ultima comanda, aceasta comanda tb sa trimita output-ul in pipe pt urmatoarea comanda
//dup2(pipefds[i * 2 + 1], 1) redirec. stdout (file descriptor 1) la file descriptor-ul de scriere al pipe-ului creat pt aceasta comanda (pipefds[i * 2 + 1]).
                    dup2(pipefds[i * 2 + 1], 1);
                for (int j = 0; j < 2 * (num_cmds - 1); j++) 
                    close(pipefds[j]); //inchidem toate pipe-urile

                execvp(commands[i][0], commands[i]);
                perror("execvp");
                exit(EXIT_FAILURE);
            } 
            else if (pid < 0) 
            {
                perror("fork");
                exit(EXIT_FAILURE);
            }
        }

        for (int i = 0; i < 2 * (num_cmds - 1); i++) 
            close(pipefds[i]);

        for (int i = 0; i < num_cmds; i++) 
            wait(NULL);
    }

    return 0;
}
