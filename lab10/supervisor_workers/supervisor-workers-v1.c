#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

void calcul_tata(int N)
{
	int fd_operanzi=open("operanzi.bin", O_RDONLY);
	int fd_operatori=open("operatori.txt", O_RDONLY);
	
	if(-1==fd_operanzi || -1==fd_operatori)
	{
	    perror("Eroare la open");
	    exit(1);
	}
	char operator;
	int operand1, operand2, rez;
	ssize_t bytes_read_operand1, bytes_read_operand2, bytes_read_operator;
	for(int i=0; i<N; i++)
	{
	    bytes_read_operand1=read(fd_operanzi, &operand1, sizeof(int));
	    bytes_read_operand2=read(fd_operanzi, &operand2, sizeof(int));
	    bytes_read_operator=read(fd_operatori, &operator, sizeof(char));
	    if(bytes_read_operand1!=sizeof(int)||bytes_read_operand2!=sizeof(int)||bytes_read_operator!=sizeof(char))
	    {
	        perror("Eroare la read");
	        exit(1);
	    }
	    switch(operator)
	    {
	        case '+':
	            rez=operand1+operand2;
	            break;
	        case '-':
	            rez=operand1-operand2;
	            break;
	        case '*':
	            rez=operand1*operand2;
	            break;
	        case '/':
	            rez=operand1/operand2;
	            break;
	        default:
	            perror("Operator invalid");
	            exit(1);
	    }
	    printf("%d %c %d=%d\n", operand1, operator, operand2, rez);
	}
	if(-1==close(fd_operanzi)||-1==close(fd_operatori))
	{
	    perror("Eroare la close");
        exit(1);
	}
}

void generare_fiu1(int N)
{
	int fd_operanzi;
	if(-1==(fd_operanzi=open("operanzi.bin", O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR)))
	{
	    perror("Eroare la open");
	    exit(1);
	}
	int operanzi[2];
	ssize_t bytes_written;
	for(int i=0; i<N; i++)
	{
	    operanzi[0]=rand()%100;
	    operanzi[1]=rand()%100;
	    if(-1==(bytes_written=write(fd_operanzi, operanzi, sizeof(operanzi))))
	    {
	        perror("Eroare la write");
	        exit(1);
	    }
	}
	if(-1==close(fd_operanzi))
	{
	    perror("Eroare la close");
	    exit(1);
	}
	
}

void generare_fiu2(int N)
{
	int fd_operatori;
	if(-1==(fd_operatori=open("operatori.txt", O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR)))
	{
	    perror("Eroare la open");
	    exit(1);
	}
	char operatori[]={'+', '-', '*', '/'};
	ssize_t bytes_written;
	for(int i=0; i<N; i++)
	{
	    char op=operatori[rand()%4];
	    if(-1==(bytes_written=write(fd_operatori, &op, sizeof(char))))
	    {
	        perror("Eroare la write");
	        exit(1);
	    }
	}
	if(-1==close(fd_operatori))
	{
	    perror("Eroare la close");
	    exit(1);
	}
}

int N=0;

int main()
{
	pid_t pid_fiu1, pid_fiu2;

	printf("Dati numarul intreg N:");
	scanf("%d", &N);
	if(N<1)
	{
	    fprintf(stderr, "Numarul nu este pozitiv\n");
	    exit(1);
	}

	/* Crearea procesului fiu #1. */
	if(-1 == (pid_fiu1=fork()) )
	{
		perror("Eroare la fork #1");  return 1;
	}

	/* Ramificarea execuției după primul apel fork. */
	if(pid_fiu1 == 0)
	{   /* Zona de cod executată doar de către fiul #1. */
		printf("\n[P1] Procesul fiu 1, cu PID-ul: %d.\n", getpid());

		generare_fiu1(N);
		return 0;
	}
	else
	{   /* Zona de cod executată doar de către părinte. */
		printf("\n[P0] Procesul tata, cu PID-ul: %d.\n", getpid());

		/* Crearea procesului fiu #2. */
		if(-1 == (pid_fiu2=fork()) )
		{
			perror("Eroare la fork #2");  return 2;
		}
		
		/* Ramificarea execuției după al doilea apel fork. */
		if(pid_fiu2 == 0)
		{   /* Zona de cod executată doar de către fiul #2. */
			printf("\n[P2] Procesul fiu 2, cu PID-ul: %d.\n", getpid());

			generare_fiu2(N);
			return 0;
		}
		else
		{   /* Zona de cod executată doar de către părinte. */

			wait(NULL);
			wait(NULL);
			// Mai întâi aștept terminarea ambilor fii, iar apoi fac calculul.

			calcul_tata(N);
			return 0;
		}
	}
}

