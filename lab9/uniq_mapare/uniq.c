#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define handle_error(msg) \
{ perror(msg); exit(EXIT_FAILURE);}

int main(int argc, char*argv[])
{
    int opt;
    int print_count = 0; // opțiunea -c pentru număr de apariții
    int width = -1; // dacă nu este specificat, se compară întreaga linie

    // Procesăm opțiunile din linia de comandă
    while ((opt = getopt(argc, argv, "cw:")) != -1)
    {
        switch (opt)
        {
            case 'c':
                print_count = 1; // afișează contorul pentru liniile unice
                break;
            case 'w':
                width = atoi(optarg);
                if (width <= 0)
                {
                    handle_error("Width must be greater than 0");
                }
                break;
            default:
                return EXIT_FAILURE;
        }
    }
    
    if (optind >= argc)
    {
        handle_error("Nu ati inclus niciun fisier");
    }
    
    for (int i=optind; i<argc; i++)
    {
        int fd;
        fd=open(argv[i], O_RDONLY);
        if (fd==-1)
        {
            handle_error("Eroare la deschidere");
        }
        
        struct stat sb;
        if(fstat(fd, &sb)==-1)
        {
            handle_error("Eroare la fstat");
        }
        
        char* map_addr=mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
        if(map_addr==MAP_FAILED)
        {
            handle_error("Eroare la mmap");
        }
        
        if(close(fd)==-1)
        {
            handle_error("Eroare la inchidere");
        }
        
        char** linii=malloc(sb.st_size*sizeof(char*));
        if(linii==NULL)
        {
            handle_error("Eroare la malloc");
        }
        
        size_t nr_linii=0;
        size_t start_linie=0;
        
        for(size_t i=0; i<sb.st_size; i++)
        {
            if(map_addr[i]=='\n' || i==sb.st_size-1)
            {
                size_t lungimea_liniei=(i-start_linie)+1;
                linii[nr_linii]=malloc(lungimea_liniei*sizeof(char));
                if(linii[nr_linii]==NULL)
                {
                    handle_error("Eroare la alocarea memoriei");
                }
                strncpy(linii[nr_linii], &map_addr[start_linie], lungimea_liniei);
                linii[nr_linii][lungimea_liniei-1]='\0';
                //printf("%s\n",linii[nr_linii]);
                nr_linii++;
                start_linie=i+1;
            }
        }
        
        for(size_t i=1; i<=nr_linii; i++)
        {
            int count=1;
            if(i==nr_linii)
            {
                if(print_count==1)
                {
                    printf("%d %s\n",count, linii[i-1]);
                }
                else
                {
                    printf("%s\n",linii[i-1]);
                }
                break;
            }
            char* linie_curenta=linii[i-1];
            if(width==-1)
            {
                while(strcmp(linii[i-1], linii[i])==0)
                {
                    count++;
                    i++;   
                }
            }
            else
            {
                while(strncmp(linii[i-1], linii[i], width)==0)
                {
                    count++;
                    i++;   
                }
            }
            i--;
            if(print_count==1)
            {
                printf("%d %s\n",count, linie_curenta);
            }
            else
            {
                printf("%s\n",linie_curenta);
            }
            i++;
        }
        
        for(size_t i=0; i<nr_linii; i++)
        {
            if(linii[i]!=NULL)
            {
                free(linii[i]);
                linii[i]=NULL;
            }
        }
        free(linii);
        linii=NULL;
        
        if(munmap(map_addr, sb.st_size)==-1)
        {
            handle_error("Eroare la munmap");
        }
    }
}
