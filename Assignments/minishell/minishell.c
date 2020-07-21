/*******************************************************************************
 * Name        : minishell.c
 * Author      : Michael DelGaudio & Sunmin Lee
 * Date        : 06/17/2020
 * Description : Minishell
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <setjmp.h>

//Colors
#define BRIGHTBLUE "\x1b[34;1m" 
#define DEFAULT "\x1b[0m"

//Globals
sigjmp_buf jmpbuf;
sig_atomic_t interrupted = false;
static char * userHomeDir;
static int userHomeDirLength;
static bool childWIP = false;


void catch_alarm(int sig){
    putchar('\n');
    if(childWIP){
        childWIP  = false;
    } else{
        //putchar('\n');
        interrupted = true;
        siglongjmp(jmpbuf, 1);
    }
}



//String trimmer for leading or trailing whitespace
//RESOURCE:
//https://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way?page=1&tab=votes#tab-top
char *trimWhiteSpace(char *stringInput){
  char *end;
  // Trim leading space
  while(isspace((unsigned char)*stringInput)) stringInput++;
  if(*stringInput == 0)  // All spaces?
    return stringInput;
  // Trim trailing space
  end = stringInput + strlen(stringInput) - 1;
  while(end > stringInput && isspace((unsigned char)*end)) end--;
  // Write new null terminator character
  end[1] = '\0';
  return stringInput;
}

//RESOURCE:
//https://stackoverflow.com/questions/4761764/how-to-remove-first-three-characters-from-string-with-c
void chopOffNChars(char * stringInput, size_t removeAmount){
    assert(removeAmount != 0 && stringInput != 0);
    size_t len = strlen(stringInput);
    if (removeAmount > len)
        return;  // Or: n = len;
    memmove(stringInput, stringInput + removeAmount, len - removeAmount + 1);
}

//Prints error functions
void printFailedMalloc(){
    fprintf(stderr, "Error: malloc() failed. %s.\n", strerror(errno));
}
void printFailedCWD(){
    fprintf(stderr, "Error: Cannot get current working directory. %s.\n", strerror(errno));
}

//How we will utilize the exe 
static char ** childArguments;
static int childNumArgs;

//Takes in our trimmed input from the child
//Determines the respective amount of args
//Sets up the ** argument char to be utilized
int processChildInput(char * trimmedInput){
    int parserCount = 0;
    int argCount = 0;
    char ** tempArgs;
    int tempTotalArgs = 0;
    int loopCount = 0;


    while (loopCount < 2){

        while(parserCount < strlen(trimmedInput)){
            //The parser is less than our total trimmedInput

            int tempBeginPosition = parserCount;
            int sizeOfArg = 0;

            //if the current char is not a blank and we didnt fall off yet
            while(*(parserCount + trimmedInput) != ' ' && parserCount < strlen(trimmedInput)){
                parserCount += 1; //move the counter up
                sizeOfArg += 1; //that means the arg has more size
            }

            //We left the loop therefore check the arg size
            if(sizeOfArg > 0){

                if(loopCount != 0){
                    char * whatIsMyArg;
                    if((whatIsMyArg = (char *)malloc(sizeof(char) * (sizeOfArg + 1))) == NULL) {
                        printFailedMalloc();
                        for(int i = 0; i < tempTotalArgs; i++){
                            free(loopCount + tempArgs);
                        }
                        free(tempArgs);
                        return 1;
                    }
                    memcpy(whatIsMyArg, tempBeginPosition + trimmedInput, sizeOfArg); //put the arg in mem
                    *(sizeOfArg + whatIsMyArg) = '\0'; //make sure to terminate
                    *(argCount + tempArgs) = whatIsMyArg; //push to tempArgs
                    argCount += 1; //added arg to counter
                } else {
                    tempTotalArgs += 1; //we are on our first loop therefore it is increased
                }

            }

            //What if we did have a blank space?
            while(*(parserCount + trimmedInput) == ' ' && parserCount < strlen(trimmedInput)){
                //just increase the parseCount
                parserCount += 1;
            }

        }

        if(loopCount != 0){
            *(tempArgs + tempTotalArgs) = NULL;
        } else {
            if( (tempArgs = (char **)malloc(sizeof(char *) * (tempTotalArgs + 1))) == NULL ){
                printFailedMalloc();
                free(tempArgs);
                return 1;
            }
        }

        //reset and advance the loop
        parserCount = 0;
        loopCount += 1;
    }

    childNumArgs = tempTotalArgs;
    childArguments = tempArgs;
    return 0;
}

// We need to deal with the multiple scnearios of CD
char * processCD(char * trimmedInput){
   
    chopOffNChars(trimmedInput, 3);
    trimmedInput = trimWhiteSpace(trimmedInput);

    int index = 0;
    //bool weHaveQuote = false;
    int quotes_counter = 0;

    //Check if we have too many args

    for(int i = 0; i<strlen(trimmedInput); i++){
            if(trimmedInput[i] == '"'){
                quotes_counter++;
            }
    }
    if(quotes_counter == 0){
        
        while(*(trimmedInput + index) != ' ' && index < strlen(trimmedInput)){
            index++;
        }
        while(index < strlen(trimmedInput)){
            if( *(trimmedInput + index) != ' '){
                fprintf(stderr, "Error: Too many arguments to cd.\n");
                return "";
            }
            index++;
        }
    } else {
        
        //We do have quotes
        //int endingPosition = 1;
        //bool weHaveQuoteEnd = false;

        
        int word_count = 1; 
        for(int i = 1; i<strlen(trimmedInput); i++){
            if(trimmedInput[i] == ' '){
                word_count++;
            }
        }
        int index_last_quote = 0;
        for(int i = 0; i<strlen(trimmedInput); i++){
            if(trimmedInput[i] == '"'){
                index_last_quote++; 
            }
            if(index_last_quote == quotes_counter){
                index_last_quote = i;
                break;
            }
        }
        //printf("Last quote: %d\n", index_last_quote);
        //printf("words: %d\n", word_count);
        if(index_last_quote < strlen(trimmedInput) && trimmedInput[index_last_quote+1] == ' '){
            fprintf(stderr, "Error: Too many arguments to cd.\n");
            return "";
        }
        /*
        while(index < strlen(trimmedInput)){
            index++;
            if(trimmedInput[index] == '"'){
                weHaveQuoteEnd = true;
                endingPosition = index;
                printf("Index: %d\n", index);
             }
            
         }
        */
        /*
        if(quotes_counter == 2){
            if(trimmedInput[strlen(trimmedInput)-1] != '"'){
                fprintf(stderr,"Error: Malformed command.3\n");
                return "";
            }
        } else */
        if(quotes_counter%2 /*|| ((word_count > 1) && (quotes_counter != 2 * word_count))*/){
            fprintf(stderr,"Error: Malformed command.\n");
            return "";
        }
        
        if(word_count > 0)
            while(index < strlen(trimmedInput)){
                if(trimmedInput[index] == '"'){
                    //memmove(&trimmedInput[0], &trimmedInput[1], strlen(trimmedInput)-0);
                    memmove(&trimmedInput[index],&trimmedInput[index+1],strlen(trimmedInput)-(index-1));
                    index = 0;
                    //printf("Current: %s\n", trimmedInput);
                    
            } else{
                index++;
            }
            }

        // //remove beginning quote
        // memmove(&trimmedInput[0],&trimmedInput[0+1],strlen(trimmedInput)-0);

        //printf("what: %s\n", trimmedInput);
        // //remove ending quote
        //printf("%d\n", endingPosition );
        //memmove(&trimmedInput[endingPosition],&trimmedInput[endingPosition+1],strlen(trimmedInput)-endingPosition);
        
        //printf("what2: %s\n", trimmedInput);


    }

    return trimmedInput;
}

static char cwd[PATH_MAX];


//RESOURCE:
//https://stackoverflow.com/questions/298510/how-to-get-the-current-directory-in-a-c-program
int getCWD(){
   if (getcwd(cwd, sizeof(cwd)) == NULL) {
        fprintf(stderr, "Error: Cannot get current working directory. %s.\n", strerror(errno));
        return 1;
   } 
   return 0;
}


int main(int argc, char *argv[]) {

    //SIGNAL HANDLER - stops you from ctrl+c which is what we want
    struct sigaction action;    
    memset(&action, 0, sizeof(struct sigaction));    
    action.sa_handler = catch_alarm;    
    action.sa_flags = SA_RESTART; /* Restart syscalls if possible*/    
    if (sigaction(SIGINT, &action, NULL) == -1) {        
        fprintf(stderr, "Error: Cannot register signal handler. %s.\n", strerror(errno));     
        return EXIT_FAILURE;    
    }
    ////////////////////
    

    //PASSWD ENTRY
    uid_t passwordUID = getuid();
    struct passwd  * passwordPWUID;
    if(( (passwordPWUID = getpwuid(passwordUID)) == NULL)){
        fprintf(stderr, "Error: Cannot get passwd entry. %s.\n", strerror(errno));
        return EXIT_FAILURE;
    }
    userHomeDir = passwordPWUID->pw_dir;
    userHomeDirLength = strlen(userHomeDir);
    /////////////////

    //PROMT SETUP
    size_t argMaxSize = (256 * 1024);
    char * maxPromptInput = (char *)malloc(sizeof(char) * (256 * 1024));
    if (maxPromptInput == NULL) {
        printFailedMalloc();
        return EXIT_FAILURE;
    }
    ////////////////////

    //GET THE CURRENT WORKING DIRECTORY
    if (getCWD() != 0) {
        free(maxPromptInput);
        return EXIT_FAILURE;
    }
   
    ////////////////////

    sigsetjmp(jmpbuf, 1);
    bool dirNotFound = false;
   
    //PRINT THE PROMPT
    while(1){
       

        if(interrupted){
            interrupted = false;
            continue;
        }

        //If the directory wasn't previous
        if (dirNotFound == true) {
            //ensure its good
            if (getCWD() != 0) {
        
                free(maxPromptInput);
                return EXIT_FAILURE;
            }
            dirNotFound = false;
        }

        
        if ( (printf("[%s%s%s]$ ", BRIGHTBLUE, cwd, DEFAULT)) < 0){
            fprintf(stderr, "Error: Cannot printf(). %s.\n", strerror(errno));
            free(maxPromptInput);
            return EXIT_FAILURE;
        }
   

        //Grab user input
        ssize_t maxPromptInputLength = 0;
        if((maxPromptInputLength = getline(&maxPromptInput, &argMaxSize, stdin)) == -1){
            if(errno != 0){
                fprintf(stderr, "Error: Failed to read from stdin. %s.\n", strerror(errno)); 
                free(maxPromptInput);
                return EXIT_FAILURE;
            } else{
                putchar('\n');
                free(maxPromptInput);
                return EXIT_SUCCESS;
            }
        }


        //if the last char was a newline, we want to make sure its set to null terminater
        if( *((maxPromptInputLength + maxPromptInput) - 1) == '\n'){
            *((maxPromptInputLength + maxPromptInput) - 1) = '\0';
            maxPromptInputLength -= 1;
        }

       
        //We have our input but we need to trim whitespace so we don't break other funcs
        //this does modify the original string - not sure if we want to do that 
        char * trimmedInput;
        trimmedInput = trimWhiteSpace(maxPromptInput);
        if(strlen(trimmedInput) == 0){
            //IF THE USER PASSES <BLANK> INPUT
            //do we need to free anything?
            continue;

        } else if (  (strncmp(trimmedInput, "cd ", 3) == 0)  || ((strcmp(trimmedInput, "cd") == 0 && strlen(trimmedInput) == 2)) ){
            //Get the cd arg
            
            char * cleanedUpCD;
            cleanedUpCD = processCD(trimmedInput);
            /*
            if( strcmp(cleanedUpCD, "") == 0){
                continue;
            }
            */
            if(strcmp(cleanedUpCD, "~") == 0 || strcmp(cleanedUpCD, "cd") == 0 || strcmp(cleanedUpCD, "") == 0){
                // go to home dir 
                if(chdir(userHomeDir) != 0){
                    fprintf(stderr, "Error: Cannot change directory to '%s'. %s.\n", cleanedUpCD, strerror(errno));
                }
                if(getCWD() != 0){
                    fprintf(stderr, "Error: Cannot get current working directory. %s.\n", strerror(errno));
                } 

            } else if(strcmp(cleanedUpCD, "..") == 0){

                // go up one dir
          
                if(chdir("..") != 0){
                    fprintf(stderr, "Error: Cannot change directory to '%s'. %s.\n", cleanedUpCD, strerror(errno));
                }                
                if(getCWD() != 0){
                    fprintf(stderr, "Error: Cannot get current working directory. %s.\n", strerror(errno));
                }

            } else {
                // regular path
                
                if(chdir(cleanedUpCD) != 0){
                    fprintf(stderr, "Error: Cannot change directory to '%s'. %s.\n", cleanedUpCD, strerror(errno));
                   
                }
                if(getCWD() != 0){
                    fprintf(stderr, "Error: Cannot get current working directory. %s.\n", strerror(errno));
                }
            }


        } else if ( strcmp(trimmedInput, "exit") == 0 && strlen(trimmedInput) == 4 ){
            //IF THE USER WROTE 'EXIT'
            free(maxPromptInput);
            return EXIT_SUCCESS;
        } else {
            //GO TO THE CHILD PROCESS
            pid_t pidder;
            if ( (pidder = fork()) == 0){
                //in the child
                int processedChildStatus = processChildInput(trimmedInput);

                //procces input
                if (processedChildStatus == 1){
                    //FAIL
                    free(maxPromptInput);
                    return EXIT_FAILURE;
                }

                //WE ARE GOOD

                //run exe
                if( (execvp(childArguments[0], childArguments)) == -1){
                    fprintf(stderr, "Error: exec() failed. %s.\n", strerror(errno));
                    for (int i = 0; i < childNumArgs; i++) {
                        free(childArguments[i]);
                    }
                    free(childArguments);
                    free(maxPromptInput);
                    return EXIT_FAILURE;
                }

                for (int i = 0; i < childNumArgs; i++) {
                    free(childArguments[i]);
                }
                free(childArguments);

            } else if (pidder > 0){

                childWIP = true;

                //in the parent
                int state;

                if(wait(&state) == -1){
                    fprintf(stderr, "Error: wait() has failed in the parent. %s.\n", strerror(errno));
            
            
                    free(maxPromptInput);
                    return EXIT_FAILURE;
                }

                childWIP = false;

                if(0 == WEXITSTATUS(state)){
                    dirNotFound = true;
                }

            } else if (pidder < 0){
                //we just totally failed

                fprintf(stderr, "Error: fork() failed. %s.\n", strerror(errno));
                free(maxPromptInput);
                return EXIT_FAILURE;

            }

            pidder = 0;
        }
    }
    free(maxPromptInput);
    return EXIT_SUCCESS;
}