/*******************************************************************************
 * Name        : pfind.c
 * Author      : Michael DelGaudio & Sunmin Lee
 * Date        : 06/03/2020
 * Description : Discovers permissions of files in directory.
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <getopt.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <libgen.h>
#include <stdbool.h>
#include <limits.h>

//SCENARIOS TO BE CLARIFIED:
/*
-Does order of printed out files matter?
-Valgrinds
Check eventually:
- Permission denied in recursive call
- No output when there is a successful no match
*/


/**
 * Ensure user input of perms is valid syntax
 **/
int checkPermissionInput(char *pInput){
    //the length != 9 -> NO GOOD
    if(strlen(pInput) != 9){
        return 0;
    }
    int j = 0;
    for(int i = 0; i < 9; i+=3 ){

        //0, 3, 6 j-index positions to check for r 
        if(pInput[j] != 'r' && pInput[j] != '-'){
            return 0;
        } 
        j++;

        //1, 4, 7 j-index positions to check for w
        if(pInput[j] != 'w' && pInput[j] != '-'){
            return 0;
        }
        j++;

        //2, 5, 8 j-index positions to check for x
        if(pInput[j] != 'x' && pInput[j] != '-'){
            return 0;
        }
        j++;

    }
    //no issues - valid syntax string
    return 1;
}

// Returns the permission string
char* permission_string(struct stat *sBuf) {


    char *stringOfPerm=calloc(11, sizeof(char));
    int lstPerms[] = {S_IRUSR, S_IWUSR, S_IXUSR, S_IRGRP, S_IWGRP, S_IXGRP, S_IROTH, S_IWOTH, S_IXOTH};
    int permission_valid; 
    int j = 0;
    for(int i = 0; i < 9; i+=3){
        permission_valid = sBuf->st_mode & lstPerms[i];
        if(permission_valid){
            stringOfPerm[j]='r';
        } else {
            stringOfPerm[j]='-';
        }
        j++;
        permission_valid = sBuf->st_mode & lstPerms[i+1];
        if(permission_valid){
            stringOfPerm[j]='w';
        } else {
            stringOfPerm[j]='-';
        }
        j++;
        permission_valid = sBuf->st_mode & lstPerms[i+2];
        if(permission_valid){
            stringOfPerm[j]='x';
        } else {
            stringOfPerm[j]='-';
        }
        j++;
    }
    *(stringOfPerm+10)='\0';
    return stringOfPerm;
}

/**
 * The main runner of our program
 * [ ] Ensures no permission denied.
 * [ ] Recursive navigation to find directories with matching perms

 * Reference:
 * How to recurse through directories: https://stackoverflow.com/questions/8436841/how-to-recursively-list-directories-in-c-on-linux
 **/ 
void run(char *dInput, char *pInput){
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;

    
    // If directory doesn't open, there could be a bunch of reasons, have to sort out error messages 
    if(!(dir = opendir(dInput))){//not sure on dInput
        char full_path[PATH_MAX];
        
         if (errno != 2) {
            fprintf(stderr, "Error: Cannot open directory '%s'. Permission denied.\n",  realpath(dInput, full_path));
        } else if (errno == 2) {
            fprintf(stderr, "Error: Cannot stat '%s'. %s.\n",  realpath(dInput, full_path), strerror(errno));
        }
        closedir(dir);
        return;
    }

    while((entry = readdir(dir)) != NULL){
    	
        if(entry->d_type == DT_DIR){
            // When the branch u are on is a directory
            
            char path[PATH_MAX];
            char full_path[PATH_MAX];
            if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
                continue;
            }
            snprintf(path, sizeof(path), "%s/%s", dInput, entry->d_name);
            realpath(path, full_path);
            if(lstat(full_path, &statbuf) < 0){
                fprintf(stderr, "Error: Cannot stat '%s'. %s\n", full_path, strerror(errno));
                exit(1);
            }
            // store permission string
            char *stringOfPerm = permission_string(&statbuf);
            // If permission denied, then error
            int dashCounter = 0;
            for(int i = 0; i < 9; i++){
                if(stringOfPerm[i] == '-'){
                    dashCounter++;
                }
            }
            if(dashCounter == 9){
                if(strcmp(pInput, stringOfPerm) == 0){
                    printf("%s\n", full_path);
                }
                fprintf(stderr, "Error: Cannot open directory '%s'. Permission denied.\n", full_path);
                free(stringOfPerm);
                closedir(dir);
                return;
            }
            // If the permission of said file is the same, then print the file name
            
            if(strcmp(pInput, stringOfPerm) == 0){
                printf("%s\n", full_path);
            }
            run(path, pInput);
            free(stringOfPerm);
        } else if(entry->d_type == DT_LNK){
            // When ur on a sym link 
        	char path[PATH_MAX];
        	char full_path[PATH_MAX];
        	realpath(dInput, path);
        	
        	size_t pathlen = 0;
        	
        	full_path[0] = '\0';
        	if(strcmp(path, "/")){
        		strncpy(full_path, path, PATH_MAX);
        	}
        	
        	pathlen = strlen(full_path)+1;
        	full_path[pathlen-1] = '/';
        	full_path[pathlen] = '\0';
        	strncpy(full_path + pathlen, entry->d_name, PATH_MAX - pathlen);
        	if(lstat(full_path, &statbuf) < 0){
                fprintf(stderr, "Error: Cannot stat '%s'. %s\n", full_path, strerror(errno));
                closedir(dir);
                exit(1);
            }
            
            // store permission string
            char *stringOfPerm = permission_string(&statbuf);
            // If the permission of said file is the same, then print the file name
            if(strcmp(pInput, stringOfPerm) == 0){
                printf("%s\n", full_path);
            }
            free(stringOfPerm);
        } else {
            // When it's just a regular file
            char full_path[PATH_MAX];
            char path[PATH_MAX];
            snprintf(path, sizeof(path), "%s/%s", dInput, entry->d_name);
            realpath(path, full_path);
            //printf("stating %s\n", full_path);
            if(lstat(full_path, &statbuf) < 0){
                fprintf(stderr, "Error: Cannot stat '%s'. %s\n", full_path, strerror(errno));
                closedir(dir);
                exit(1);
            }
            // store permission string
            char *stringOfPerm = permission_string(&statbuf);
            // If the permission of said file is the same, then print the file name
            if(strcmp(pInput, stringOfPerm) == 0){
                printf("%s\n", full_path);
            }
            free(stringOfPerm);
        }
    }
    closedir(dir);      
    }


/**
 * Process user input and call upon other functions.
 **/ 
int main(const int argc, char * argv[]) {

    //Check inital argument amount
    if (argc == 1){
       printf("Usage: %s -d <directory> -p <permissions string> [-h]\n", argv[0]);
       return EXIT_FAILURE;
    }

    //declare input variables
    char input;
    char path[PATH_MAX];
    char *pInput;
    char *dInput;
    bool dFlag = false;
    bool pFlag = false;

    //let's grab our flags with getopts
    while ((input = getopt(argc, argv, ":d:p:h")) != -1) {
        switch (input) {
            case 'd':
                //directory    
                dFlag = true;        
                dInput = optarg;
                break;
            case 'p':
                //permissions
                pFlag = true;
                pInput = optarg;
                break;
            case 'h':
                //help
                printf("Usage: %s -d <directory> -p <permissions string> [-h]\n", argv[0]);
                return EXIT_SUCCESS;
            case '?':
                printf("Error: Unknown option '-%c' received.\n", optopt);
                return EXIT_FAILURE;
            default:
                break;
            }
        
    }

    //Check flags are both active
    if (dFlag == false) {
        printf("Error: Required argument -d <directory> not found.\n");
        return EXIT_FAILURE;
    }
    if (pFlag == false) {
        printf("Error: Required argument -p <permissions string> not found.\n");
        return EXIT_FAILURE;
    }

    //Check if we are able to stat directory 
    if (realpath(dInput, path) == NULL){
        printf("Error: Cannot stat '%s'. No such file or directory.\n", dInput);
        return EXIT_FAILURE;
    }
    

    int statusForPInput = checkPermissionInput(pInput);
    //Check user input syntax permissions
    if(statusForPInput == 1){
        //we are good
        //LETS RUN
        run(dInput, pInput);
    } else{
        //we are bad
        printf("Error: Permissions string '%s' is invalid.\n", pInput);
        return EXIT_FAILURE;
    }

    //tbd if we would leave sucls
    
    return EXIT_SUCCESS;
}
