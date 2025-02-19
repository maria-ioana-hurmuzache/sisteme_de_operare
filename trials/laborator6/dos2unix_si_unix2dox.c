/*Un program de tip filtru este un program care copie conținutul unui fișier de intrare într-un fisier de ieșire, aplicând în timpul copierii o anumită transformare asupra conținutului copiat.
Exemplu: comenzile dos2unix și unix2dos.
Explicație: fișierele cu text obișnuit folosesc caracterul newline (reprezentat prin '\n' în limbajul C), ca și separator de linie. Numai că reprezentarea caracterului newline este dependentă de platformă, i.e. de SO-ul pe care ați creat acel fișier, cu ajutorul unui program de tipul editor de text obișnuit pentru acea platformă!
Comanda dos2unix filtrează fișierul de intrare, aplicându-i translatarea reprezentării caracterului newline folosită de SO-urile DOS și Windows, la reprezentarea caracterului newline folosită de Linux și celelalte SO-uri din familia UNIX. Iar comanda unix2dos realizează filtrarea în sens invers.

Cerință: să se scrie un program C care primește de la linia de comandă numele a două fișiere și care va copia conținutul fișierului de intrare în cel de ieșire, transformând conținutul copiat conform conversiei realizate de comanda dos2unix. Similar, să se scrie un program C care simulează comanda unix2dos.*/

/*
* Program: dos2unix.c
*
* Funcționalitate: filtru pentru fișiere text, conversie de la newline în DOS/Windows (ASCII 13 + ASCII 10) la newline în UNIX/Linux (ASCII 10).
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define BLOCK_SIZE 4096  /* !!! exact dimensiunea paginii de memorie, din motive de eficiență a operațiilor cu discul !!! */

int main(int argc,char *argv[])
{
  char  bufferIn[BLOCK_SIZE], bufferOut[BLOCK_SIZE];
  char *pBufferIn,*pBufferOut;
  int   i,codRead,fdIn,fdOut;

  if (argc != 3)
  {
     fprintf(stderr,"\nUsage:  %s  input-file[DOS textfile]  output-file[UNIX textfile]\n\n",argv[0]);
     exit(1);
  }
 
  /* pregatirea fisierelor pentru prelucrare */
  printf("Input file is: %s\n",argv[1]); 
  if(-1 == (fdIn = open(argv[1],O_RDONLY)) )
  {
    perror("Eroare la apelul open");
    exit(2);
  }

  printf("Output file is: %s\n",argv[2]); 
  if(-1 == (fdOut = creat(argv[2],0600)) )
  {
    perror("Eroare la apelul creat");
    exit(3);
  }

  /* urmeaza prelucrarea efectiva: conversia specificata mai sus */
  while (0 != (codRead = read(fdIn,bufferIn,BLOCK_SIZE)) )
  {
    if (-1 == codRead)
    {
      perror("Eroare la apelul read");
      exit(4);
    } 

    pBufferIn  = bufferIn;
    pBufferOut = bufferOut;

    for(i=0; i < codRead; i++,pBufferIn++)
    {
      if (*pBufferIn != '\015')  /* caracterul ASCII 13, il ignoram */
      {
        *pBufferOut = *pBufferIn;
        pBufferOut++;
      }
    }

    int nrBytesOut = pBufferOut - bufferOut;
    if (-1 == write(fdOut,bufferOut,nrBytesOut) )
    {
      perror("Eroare la apelul write");
      exit(5);
    } 
  }/* endwhile */

  close(fdIn);
  close(fdOut);

  printf("Done! The conversion was succesfully terminated.\n");
  return 0;
}

///al 2-lea program

/*
* Program: unix2dos.c
*
* Funcționalitate: filtru pentru fișiere text, conversie de la newline în UNIX/Linux (ASCII 10) la newline în DOS/Windows (ASCII 13 + ASCII 10).
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define BLOCK_SIZE 4096  /* !!! exact dimensiunea paginii de memorie, din motive de eficiență a operațiilor cu discul !!! */

int main(int argc,char *argv[])
{
  char  bufferIn[BLOCK_SIZE], bufferOut[BLOCK_SIZE*2];
  char *pBufferIn,*pBufferOut;
  int   i,codRead,fdIn,fdOut;

  if (argc != 3)
  {
     fprintf(stderr,"\nUsage:  %s  input-file[UNIX textfile]  output-file[DOS textfile]\n\n",argv[0]);
     exit(1);
  }
 
  /* pregatirea fisierelor pentru prelucrare */
  printf("Input file is: %s\n",argv[1]); 
  if(-1 == (fdIn = open(argv[1],O_RDONLY)) )
  {
    perror("Eroare la apelul open");
    exit(2);
  }

  printf("Output file is: %s\n",argv[2]); 
  if(-1 == (fdOut = creat(argv[2],0600)) )
  {
    perror("Eroare la apelul creat");
    exit(3);
  }

  /* urmeaza prelucrarea efectiva: conversia specificata mai sus */
  while (0 != (codRead = read(fdIn,bufferIn,BLOCK_SIZE)) )
  {
    if (-1 == codRead)
    {
      perror("Eroare la apelul read");
      exit(4);
    } 

    pBufferIn  = bufferIn;
    pBufferOut = bufferOut;

    for(i=0; i < codRead; i++,pBufferIn++,pBufferOut++)
    {
      if (*pBufferIn == '\012') /* caracterul ASCII 10, il prefixam cu ASCII 13 */
      {
        *pBufferOut = '\015';
        pBufferOut++;
      }
      *pBufferOut = *pBufferIn;
    }

    int nrBytesOut = pBufferOut - bufferOut;
    if (-1 == write(fdOut,bufferOut,nrBytesOut) )
    {
      perror("Eroare la apelul write");
      exit(5);
    } 
  }/* endwhile */

  close(fdIn);
  close(fdOut);

  printf("Done! The conversion was succesfully terminated.\n");
  return 0;
}


