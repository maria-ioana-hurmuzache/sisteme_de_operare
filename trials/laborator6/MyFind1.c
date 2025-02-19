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

int afisare_fileinfo(char* cale)
{
    struct stat st;
    struct passwd *pwd;
    struct group *grp;
    char perm[10]="---------";
    int result=0;
    if (0!=stat(cale, &st))
    {
        fprintf(stderr,"Eroare la stat pt %s\n", cale);
        return 2;
    }
    printf("Analizam fisierul/directorul %s\n", cale);
    printf("\t Tipul fisierului este: ");
    switch(st.st_mode & S_IFMT)
    {
        case S_IFDIR: printf("Directory\n");result=1;break;
        case S_IFREG: printf("Regular file\n");break;
        case S_IFLNK: printf("Link\n");break;
        case S_IFIFO: printf("FIFO\n");break;
        case S_IFSOCK: printf("Socket\n");break;
        case S_IFBLK: printf("Block device\n");break;
        case S_IFCHR: printf("Character device\n");break;
        default: printf("Unknown\n");
    }
    printf("\tNr de nume alternative: %ld octeti\n", st.st_nlink);
    printf("\tSpatiul ocupat de disc pentru stocarea acestuia:%lld sectoare\n", (long long)st.st_blocks);
    printf("\tPermisiunile acestuia in notatie octala sunt: %o\n", st.st_mode &0777);
    if(S_IRUSR & st.st_mode) perm[0]='r';
    if(S_IWUSR & st.st_mode) perm[1]='w';
    if(S_IXUSR & st.st_mode) perm[2]='x';
    if(S_IRGRP & st.st_mode) perm[3]='r';
    if(S_IWGRP & st.st_mode) perm[4]='w';
    if(S_IXGRP & st.st_mode) perm[5]='x';
    if(S_IROTH & st.st_mode) perm[6]='r';
    if(S_IWOTH & st.st_mode) perm[7]='w';
    if(S_IXOTH & st.st_mode) perm[8]='x';
    printf("\tPermisiuni: %s\n",perm);
    if(NULL!=(pwd=getpwuid(st.st_uid)))
        printf("\tProprietar: %s, UID: %ld\n", pwd->pw_name, (long)st.st_uid);
    else
        printf("\tProprietarul cu UID: %ld\n", (long)st.st_uid);
    if(NULL!=(grp=getgrgid(st.st_gid)))
        printf("\tGrupul proprietar: %s, GID: %ld\n", grp->gr_name, (long)st.st_gid);
    else
        printf("\tProprietarul cu GID: %ld\n", (long)st.st_gid);
    return result;
}

void parcurgere_director(char* cale)
{
    DIR *dir;
    struct dirent *de;
    char nume[PATH_MAX];
    int isFolder;
    isFolder=afisare_fileinfo(cale);
    if(isFolder!=1)
        return;
    else
    {
        if(NULL==(dir=opendir(cale)))
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
                parcurgere_director(nume);
            }
        }
        closedir(dir);
    }
}

int main(int argc, char* argv[])
{
    struct stat st;
    if(1==argc)
    {
        fprintf(stderr,"Nu ati introdus niciun argument\n");
        exit(1);
    }

    if(0!=stat(argv[1], &st))
    {
        fprintf(stderr, "Eroare la stat\n");
        perror("Cauza este: ");
        exit(2);
    }
    
    if(!S_ISDIR(st.st_mode))
    {
        fprintf(stderr, "Argumentul introdus nu e director\n");
        exit(3);
    }
    
    printf("Analizam directorul %s", argv[1]);
    parcurgere_director(argv[1]);
    return 0;
}
