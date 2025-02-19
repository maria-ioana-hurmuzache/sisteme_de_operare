#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>

void find_file(char* filename, char* directory)
{
    DIR *dir;
    struct dirent* de;
    char cale[PATH_MAX];
    struct stat st;
    
    if (NULL==(dir=opendir(directory)))
    {
        fprintf(stderr, "Eroare la deschiderea directorului %s\n", directory);
        perror("Cauza este: ");
        return;
    }

    while(NULL!=(de=readdir(dir)))
    {
       sprintf(cale, "%s/%s", directory, de->d_name);
       if(strcmp(de->d_name, filename)==0)
           printf("%s\n", cale);
       if(-1==(stat(cale, &st)))
       {
           perror("Eroare la citire cauzata de");
           return;
       }
       if(S_ISDIR(st.st_mode) && strcmp(de->d_name, ".") && strcmp(de->d_name, ".."))
           find_file(filename, cale);
    }

    closedir(dir);
}

int main(int argc, char* argv[])
{
    if(argc!=2)
    {
        fprintf(stderr, "Programul necesita un argument!");
        exit(1);
    }
    
    char* filename=argv[1];
    char* home_dir=getenv("HOME");
    
    find_file(filename, home_dir);
}
