#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <limits.h>

void parcurgere_director(const char* cale)
{
    DIR *dir=opendir(cale);
    struct dirent *de;
    char nume[PATH_MAX];
        if(NULL==dir)
        {
            fprintf(stderr, "Eroare la deschiderea directorului %s.\t", cale);
            perror("Cauza este:");
            return;
        }
        while(NULL!=(de=readdir(dir)))
        {
            if(strcmp(de->d_name,".") && strcmp(de->d_name,".."))
            {
                sprintf(nume,"%s/%s",cale,de->d_name);
                if (de->d_type==DT_LNK)
                {
                    if(access(nume, F_OK)==-1)
                    {
                        printf("Sterg legatura %s\n", nume);
                        if (unlink(nume)!=0)
                        {
                            perror("unlink");
                        }
                    }
                }
                else if (de->d_type==DT_DIR)
                {
                    parcurgere_director(nume);
                }
            }
        }
        closedir(dir);
}

int main(int argc, char*argv[])
{
    if (argc!=2)
    {
        fprintf(stderr, "Nu ati introdus un argument\n");
        return 1;
    }
    
    struct stat st;
    
    if (-1==stat(argv[1],&st))
    {
        fprintf(stderr, "Am obtinut o eroare la functia stat\n");
        return 2;
    }
    
    if ((st.st_mode & S_IFMT)!=S_IFDIR)
    {
        fprintf(stderr, "Argumentul introdus nu este un director\n");
        return 3;
    }
    
    parcurgere_director(argv[1]);

}

