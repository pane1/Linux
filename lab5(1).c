#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <grp.h>
#include <pwd.h>

#include<string.h>
#include<dirent.h>

void fileInfo(char *argv){ 
    struct stat file;
    struct group *grp;
    struct passwd *pwd;

    printf("Directory/file: '%s' \n\n", argv);

    if(stat(argv, &file) == -1){ //error check of the directory/file
        printf("Error with stat()\n");
        return;
    }

    grp = getgrgid(file.st_gid); //gets the group owner name
    pwd = getpwuid(file.st_uid); //gets the owner name

    printf("File information: ************************\n\n");

    //permissions
    printf("Mode permissions: "); 
    printf( (S_ISDIR(file.st_mode)) ? "d" : "-");
    printf( (file.st_mode & S_IRUSR) ? "r" : "-");
    printf( (file.st_mode & S_IWUSR) ? "w" : "-");
    printf( (file.st_mode & S_IXUSR) ? "x" : "-");
    printf( (file.st_mode & S_IRGRP) ? "r" : "-");
    printf( (file.st_mode & S_IWGRP) ? "w" : "-");
    printf( (file.st_mode & S_IXGRP) ? "x" : "-");
    printf( (file.st_mode & S_IROTH) ? "r" : "-");
    printf( (file.st_mode & S_IWOTH) ? "w" : "-");
    printf( (file.st_mode & S_IXOTH) ? "x" : "-");
    printf("\n");
    printf("Mode: %jo (octal)\n", (uintmax_t)file.st_mode); //permission in octal
    printf("Number of links: %ju \n", file.st_nlink); //number of links
    printf("Group name: %s\n", grp->gr_name); //gets the group owner name
    printf("Owners name: %s\n", pwd->pw_name); //gets the owerns name
    printf("Size: %jd (bytes)\n", (intmax_t)file.st_size); //gets the file size in byte units
    printf("Size: %jd (blocks)\n", (intmax_t)file.st_blksize); //gets the file size in block units
    printf("Last modified: %s\n", ctime(&file.st_mtime)); //gets the date of the last modified 
}

void traverse(char directory[100]){
    DIR* dir;
    struct dirent *ent;
    struct stat states;

    char *str = strtok(directory, "//"); //splits the parameter
    char path[] = "/"; //path string

    while(str != NULL){ //creates the file path in the correct format
        strcat(path, str);
        strcat(path, "/");
        str = strtok(NULL, "//");
    }

    printf("Directory Traversal***********************\n\n");
    
    path[strlen(path) - 1] = '\0'; 
    printf("Directory/file: '%s' \n", path);

    dir = opendir(path); //opens the file path
    if (!dir){ //error check openning the file path
        printf("Error with opendir()\n");
        return;
    }

    while((ent=readdir(dir)) != NULL){ //reads the file paths
        stat(ent->d_name,&states); //gets the stat() information
        if(!strcmp(".", ent->d_name) || !strcmp("..", ent->d_name)){ //checks for the "."
                continue;
        }
        else{
            printf("%s/%s\n",path,ent->d_name); //goes through the path
            if(S_ISDIR(ent->d_type & DT_DIR)){ //checks if its a file directory
                char newPath[100]; //string to hold the new path
                strcpy(newPath,path); //copy the file path
                strcat(newPath,"/"); 
                strcat(newPath,ent->d_name);
                traverse(newPath); //iterate throuh the file path
            }
        }
    }
    closedir(dir); //closes the directory
}

int main(int argc, char *argv[]){
    
    if(argc != 2){ //error check for the argument
        printf("Error with argc \n");
        return 0;
    }
    
    fileInfo(argv[1]); //calls for file information
    traverse(argv[1]); //calls the sub directories of the directory

    return 0;
}