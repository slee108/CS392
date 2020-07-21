/*******************************************************************************
 * Name        : permstat.c
 * Author      : Michael DelGaudio & Sunmin Lee
 * Date        : 06/08/2020
 * Description : Practice with lab5.
 * Pledge : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int perms[] = {S_IRUSR, S_IWUSR, S_IXUSR,
               S_IRGRP, S_IWGRP, S_IXGRP,
               S_IROTH, S_IWOTH, S_IXOTH};

void display_usage(char *progname) {
    printf("Usage: %s <filename>\n", progname);
}

/**
 * TODO
 * Returns a string (pointer to char array) containing the permissions of the
 * file referenced in statbuf.
 * Allocates enough space on the heap to hold the 10 printable characters.
 * The first char is always a - (dash), since all files must be regular files.
 * The remaining 9 characters represent the permissions of user (owner), group,
 * and others: r, w, x, or -.
 */
char* permission_string(struct stat *statbuf) {
    char *perm_str=calloc(11, sizeof(char));
    //int perms[] = {S_IRUSR, S_IWUSR, S_IXUSR, S_IRGRP, S_IWGRP, S_IXGRP, S_IROTH, S_IWOTH, S_IXOTH};
    int permission_valid; 
    int j = 1;
    perm_str[0] = '-';
    for(int i = 0; i < 9; i+=3){
        permission_valid = statbuf->st_mode & perms[i];
        if(permission_valid){
            perm_str[j]='r';
        } else {
            perm_str[j]='-';
        }
        j++;
        permission_valid = statbuf->st_mode & perms[i+1];
        if(permission_valid){
            perm_str[j]='w';
        } else {
            perm_str[j]='-';
        }
        j++;
        permission_valid = statbuf->st_mode & perms[i+2];
        if(permission_valid){
            perm_str[j]='x';
        } else {
            perm_str[j]='-';
        }
        j++;
    }
    *(perm_str+10)='\0';
    return perm_str;

}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        display_usage(argv[0]);
        return EXIT_FAILURE;
    }

    struct stat statbuf;
    if (stat(argv[1], &statbuf) < 0) {
        fprintf(stderr, "Error: Cannot stat '%s'. %s.\n", argv[1],
                strerror(errno));
        return EXIT_FAILURE;
    }

    /* TODO
     * If the argument supplied to this program is not a regular file,
     * print out an error message to standard error and return EXIT_FAILURE.
     * For example:
     * $ ./permstat iamdir
     * Error: 'iamdir' is not a regular file.
     */
    if(S_ISDIR(statbuf.st_mode)){
        fprintf(stderr, "Error: '%s' is not a regular file.\n", argv[1]);
        return EXIT_FAILURE;
    }

    char *perms = permission_string(&statbuf);
    printf("Permissions: %s\n", perms);
    free(perms);

    return EXIT_SUCCESS;
}
