/*******************************************************************************
 * Name        : spfind.c
 * Author      : Michael DelGaudio & Sunmin Lee
 * Date        : 06/13/2020
 * Description : Sorted Pfind assignment
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/

#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

//TODO:
// ./spfind -d / -p rwxr-xrwx does not return Total : 0
// on successful output for pfind we are not using its return value

/**
 * Allows us to check if the item should be counted in the match or not - we do not want to include Usage:
 */ 

bool usageFound = false;
int noUsage(char *buffer){
    char *avoidThis = "Usage:";
    for (int i = 0; i < strlen(avoidThis); i++){
        if(avoidThis[i] != buffer[i]){
            return 0;
        }
    }
    usageFound = true;
    return 1;
}



/**
 * Performs sorted pfind on our process by just pipes
 */ 
int main (int argc, char * argv[]){
    //Does PFIND EXE Exist?

    int sort2Parent[2];
    int pfind2Sort[2];
    //declare pid
    pid_t pid[2];

    //check if we can make the pipes successfully
    if(pipe(sort2Parent) < 0){
        fprintf(stderr, "Error: Failed to make pipe sort2Parent. %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if(pipe(pfind2Sort) < 0){
        fprintf(stderr, "Error: Failed to make pipe pfind2Sort. %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if((pid[0] = fork()) == 0){
        if ( (close(pfind2Sort[0])) == -1){
            fprintf(stderr, "Error: Could not close pfind2Sort[0] in pfind process. %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        if ( (close(sort2Parent[0])) == -1){
            fprintf(stderr, "Error: Could not close sort2Parent[0] in pfind process. %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        if ( (close(sort2Parent[1])) == -1){
            fprintf(stderr, "Error: Could not close sort2Parent[1] in pfind process. %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        
        if ( (dup2(pfind2Sort[1], STDOUT_FILENO)) == -1){
            fprintf(stderr, "Error: Could not dup2 pfind2Sort[1] in pfind process. %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        if ((execv("./pfind", argv)) == -1) {
            //THIS SHOULD PRINT IF PFIND EXE DOES NOT EXIST
            fprintf(stderr, "Error: pfind failed.\n");
            exit(EXIT_FAILURE);
        } 
    }
    //we are in the child #1
    if( (pid[1] = fork()) == 0 ){
        // int state;
        // if (wait(&state) < 0) {
        //      fprintf(stderr, "Error: Waiting failed in sort. %s\n", strerror(errno));
        //      exit(EXIT_FAILURE);
        // }
        // if (WEXITSTATUS(state) != EXIT_SUCCESS) {
        //     //fprintf(stderr, "Error: pfind failed\n");
        //     exit(EXIT_FAILURE);
        // }

        if( (close(pfind2Sort[1])) == -1){
            fprintf(stderr, "Error: Could not close pfind2Sort[1] in sort process. %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        if( (close(sort2Parent[0])) == -1) {
            fprintf(stderr, "Error: Could not close sort2Parent[0] in sort process. %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        if ( (dup2(pfind2Sort[0], STDIN_FILENO)) == -1 ){
            fprintf(stderr, "Error: Could not dup2 pfind2Sort[0] in sort process. %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        if ( (dup2(sort2Parent[1], STDOUT_FILENO)) == -1){
            fprintf(stderr, "Error: Could not dup2 sort2Parent[1] in sort process. %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        if((execlp("sort", "sort", NULL)) == -1){
            fprintf(stderr, "Error: sort failed.\n");
            exit(EXIT_FAILURE);
        } 
    } 
    //we are in the parent

    if( (close(sort2Parent[1])) == -1 ){
        fprintf(stderr, "Error: Could not close sort2Parent[1] in parent. %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if( (close(pfind2Sort[0])) == -1) {
        fprintf(stderr, "Error: Could not close pfind2Sort[0] in parent. %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if( (close(pfind2Sort[1])) == -1) {
        fprintf(stderr, "Error: Could not close pfind2Sort[1] in parent. %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if( (dup2(sort2Parent[0], STDIN_FILENO)) == -1){
        fprintf(stderr, "Error: Could not dup2 sort2parent[0] in parent. %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    char sArray[1];
    size_t bytesRead = 0;
    size_t count = 0; 
    char buffer[PATH_MAX];
    int foundCount = 0;
    

    while((bytesRead = read(STDIN_FILENO, &sArray, 1)) > 0){
        if ( (write(STDOUT_FILENO, &sArray, bytesRead)) != bytesRead){
            fprintf(stderr, "Error: Write failed. %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        
        if(sArray[0] == '\n'){
            //we hit a new line
            count = 0;
            if (noUsage(buffer) == 0){
                foundCount += 1;
            }
        }
        buffer[count] = sArray[0];
        //increment
        count += 1;

    }

    if (bytesRead == -1){
        perror("read()");
        exit(EXIT_FAILURE);
    }


    int state;
    int state2;

    if (wait(&state) < 0) {
        fprintf(stderr, "Error: Waiting failed in the parent. %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (wait(&state2) < 0) {
        fprintf(stderr, "Error: Waiting failed in the parent. %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }


    if (WEXITSTATUS(state) == 0 && WEXITSTATUS(state2) == 0) {
        if(   foundCount != 0 || !usageFound ){
            printf("Total matches: %d\n", foundCount);
        }
        exit(EXIT_SUCCESS);
    } else {
        exit(EXIT_FAILURE);
    }
        
}



