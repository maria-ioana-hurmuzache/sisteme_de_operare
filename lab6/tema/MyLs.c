#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include <libgen.h> //pt basename

void afisare_fileinfo(const char *filename, int detaliat)
{
    struct stat st;
    struct passwd *pwd;
    struct group *grp;

    if (-1==stat(filename, &st))
    {
        perror("Eroare la obtinerea informatiilor despre fisier");
        return;
    }

    if (1==detaliat)
    {
	    //Permisiuni
	    printf((S_ISDIR(st.st_mode)) ? "d" : "-");
	    printf((st.st_mode & S_IRUSR) ? "r" : "-");
	    printf((st.st_mode & S_IWUSR) ? "w" : "-");
	    printf((st.st_mode & S_IXUSR) ? "x" : "-");
	    printf((st.st_mode & S_IRGRP) ? "r" : "-");
	    printf((st.st_mode & S_IWGRP) ? "w" : "-");
	    printf((st.st_mode & S_IXGRP) ? "x" : "-");
	    printf((st.st_mode & S_IROTH) ? "r" : "-");
	    printf((st.st_mode & S_IWOTH) ? "w" : "-");
	    printf((st.st_mode & S_IXOTH) ? "x\t" : "-\t");

	    // Numărul de legături
	    printf("%ld", (long) st.st_nlink);
    
	    //Numele proprietarului
	    if(NULL!=(pwd=getpwuid(st.st_uid)))
	        printf("\t%s", pwd->pw_name);
	    else
	        printf("\tunknown");
    
	    //Numele grupului
	    if(NULL!=(grp=getgrgid(st.st_gid)))
	        printf("\t%s", grp->gr_name);
	    else
	        printf("\tunknown");

	    // Dimensiunea fisierului
	    printf("\t%lld", (long long) st.st_size);

	    // Data și ora ultimei modificări
	    struct tm *tm = localtime(&st.st_mtime);
	    char date[100];
	    strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", tm);
	    printf("\t%s", date);
    }

    // Numele fișierului
    printf("\t%s\n", basename((char*)filename));
}

void ls(const char* directory, int all, int detaliat)
{
    DIR *dir;
    struct dirent* de;
    if (NULL==(dir=opendir(directory)))
    {
        perror("Eroare la deschiderea directorului");
        return;
    }

    while (NULL!=(de=readdir(dir)))
    {
        if (all==0 && de->d_name[0] == '.') ///verific daca fisierul curent este ascuns si nu trebuie afisat
            continue;//daca da sar peste el=>trec la urmatoarea iterare a buclei while

        char cale[PATH_MAX];
        sprintf(cale, "%s/%s", directory, de->d_name);
        afisare_fileinfo(cale, detaliat);
    }

    closedir(dir);
}

int main(int argc, char* argv[])
{
    int all = 0;
    int detaliat = 0;
    char* directory = ".";

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0)
            detaliat = 1;
        else if (strcmp(argv[i], "-A") == 0)
            all = 1;
        else
            directory = argv[i];
    }

    ls(directory, all, detaliat);

    return 0;
}

