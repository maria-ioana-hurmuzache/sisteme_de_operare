/*
* Program: access_v4.c
*
* Funcționalitate: versiunea 4 (doar în varianta cu lacăt pus în mod blocant) a programului demonstrativ
*                  de acces concurent/exclusiv a două procese la un fișier dat.
*/
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

int main(int argc, char* argv[])
{
    int fd, codRead;
    unsigned char ch;
    off_t poz;
    struct flock lacat;

    /* Verificarea existenței argumentului în linia de comandă. */
    if(argv[1] == NULL)
    {
        fprintf(stderr,"Programul %s trebuie apelat cu cel putin un parametru.\n", argv[0]);  exit(1);
    }

    /* Deschiderea fișierului de date. */
    if(-1 == (fd=open("fis.dat", O_RDWR)) )
    {   /* Tratează cazul de eroare la deschidere. */
        perror("Eroare, nu pot deschide fisierul fis.dat deoarece ");
        exit(2);
    }

    /* Pregătirea unui lacăt pentru fișier, pus DOAR pe octetul/caracterul dinaintea poziției curente. */
    lacat.l_type   = F_WRLCK;
    lacat.l_whence = SEEK_CUR;
    lacat.l_start  = -1;
    lacat.l_len    = 1; /* Blocăm DOAR octetul/caracterul de la poziția anterioară celei curente în fișier. */

    /* Bucla de parcurgere a fișierului, citind caracter cu caracter până la EOF. */
    while(0 != (codRead = read(fd,&ch,1)) )
    {
        if (-1 == codRead) { perror("Eroare la apelul read"); exit(3); } /* Tratează cazul de eroare la citire. */

        if(ch == '#')
        {   /* Am întâlnit prima poziție cu '#' în fișierul de date. */


            /* O singură încercare de punere a lacătului în mod blocant  (i.e., apelul intră în așteptare până când reusește). */
            printf("\n[ProcesID:%d] Incep apelul de punere a blocajului in mod blocant, pe pozitia de dupa primul '#' deja gasit în fisier.\n",getpid());

            if(-1 == fcntl(fd,F_SETLKW,&lacat) )
            { 
                if(errno == EINTR)
                    fprintf(stderr,"[ProcesID:%d] Eroare, apelul fcntl a fost intrerupt de un semnal...", getpid());
                else
                    fprintf(stderr,"[ProcesID:%d] Eroare la blocaj...", getpid());

                perror("\tMotivul erorii");
                exit(4);
            }
            else
                printf("[ProcesID:%d] Blocaj reusit!\n", getpid());

            sleep(5); // O pauză, pentru a forța anumite ordini la execuția concurentă a mai multor instanțe ale programului.

            /* Înainte de a face suprascrierea, dar după o blocare reușită,
               mai trebuie verificat încă o dată dacă caracterul de la poziția curentă este în continuare tot '#',
               deoarece între momentul primei depistări a '#'-ului și momentul reușitei blocajului
               există posibilitatea ca acel '#' să fie suprascris de cealaltă instanță a programului, rulată în paralel cu procesul curent !!! */

            if(-1 == (poz = lseek(fd,-1L,1)) ) { perror("Eroare la apelul lseek"); exit(5); } /* Tratează cazul de eroare la repoziționare. */

            /* Recitim caracterul de la poziția unde găsisem primul '#' și, apoi, o blocasem cu succes. */

            if(-1 == read(fd,&ch,1) ) { perror("Eroare la apelul read"); exit(6); } /* Tratează cazul de eroare la citire. */

            if(ch == '#')
            {   /* A rămas tot '#' pe poziția pe care-l depistasem deja și, apoi, o blocasem... Ca atare, vom face suprascrierea lui. */

                if(-1 == lseek(fd,-1L,1) ) { perror("Eroare la apelul lseek"); exit(7); } /* Tratează cazul de eroare la repoziționare. */

                if(-1 == write(fd,argv[1],1) ) { perror("Eroare la apelul write"); exit(8); } /* Tratează cazul de eroare la scriere. */

                printf("[ProcesID:%d] Terminat. S-a inlocuit primul '#' gasit in fisierul fis.dat, la pozitia: %ld.\n", getpid(), poz);
                return 0;
            }
            else
            {
                printf("[ProcesID:%d] Notificare: caracterul '#' pe care tocmai il gasisem, la pozitia: %ld, a fost deja suprascris de cealalta instanta. "
                       "Voi cauta urmatorul '#' în fisier.\n", getpid(), poz);

                /* Deblocarea acelei poziții din fișier, unde găsisem acel caracter '#', care între timp a fost suprascris cu altceva de către cealaltă instanță. */
                lacat.l_type = F_UNLCK;
                if(-1 == fcntl(fd,F_SETLK,&lacat) ) { perror("Eroare la deblocaj"); exit(9); } /* Tratează cazul de eroare la deblocare. */
                lacat.l_type = F_WRLCK;

                /* Urmează să reiau bucla de căutare a '#'-ului ... */
            }

        } //end if

    } // end bucla while
  
    close(fd);
    printf("[ProcesID:%d] Terminat. Nu exista '#' in fisierul fis.dat !\n", getpid());
    return 0;
}
