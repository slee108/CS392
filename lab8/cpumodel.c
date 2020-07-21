/*******************************************************************************
 * Name        : cpumodel.c
 * Author      : Michael DelGaudio & Sunmin Lee
 * Date        : 06/15/2020
 * Description : Main runner
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

bool starts_with(const char *str, const char *prefix) {
    /* TODO:
       Return true if the string starts with prefix, false otherwise.
       Note that prefix might be longer than the string itself.
     */
    size_t lenOfPrefix = strlen(prefix);
    if(strncmp(str, prefix, lenOfPrefix) != 0){
      return false;
    } else {
      return true;
    }

}

int main() {
    /* TODO:
       Open "cat /proc/cpuinfo" for reading.
       If it fails, print the string "Error: popen() failed. %s.\n", where
       %s is strerror(errno) and return EXIT_FAILURE.
     */
    FILE *initPOpen;
    if( (initPOpen = popen("cat /proc/cpuinfo", "r")) == NULL ){
      fprintf(stderr, "Error: popen() failed. %s.\n", strerror(errno));
      return EXIT_FAILURE;
    }


    /* TODO:
       Allocate an array of 256 characters on the stack.
       Use fgets to read line by line.
       If the line begins with "model name", print everything that comes after
       ": ".
       For example, with the line:
       model name      : AMD Ryzen 9 3900X 12-Core Processor
       print
       AMD Ryzen 9 3900X 12-Core Processor
       including the new line character.
       After you've printed it once, break the loop.
     */
    char * modelMatch = "model name";
    char arr[256];
    size_t sizeOfArr;
    while (fgets(arr, 255, initPOpen)) {
      sizeOfArr = strlen(arr);
      arr[sizeOfArr - 1] = '\0';
      if (starts_with(arr, modelMatch) == true) {
        for (int count = 1; arr[count] != '\0'; count++) {
          if (arr[count] == ' ' && arr[count - 1] == ':') {
            //Print what we got
            printf("%s\n", 1+arr+count);
            break;
          }
        }
        break;
      } else {
        //Nothing continue the loop
        continue;
      }
    }


    /* TODO:
       Close the file descriptor and check the status.
       If closing the descriptor fails, print the string
       "Error: pclose() failed. %s.\n", where %s is strerror(errno) and return
       EXIT_FAILURE.
     */
    int status = pclose(initPOpen);

    if((status) == -1){
      fprintf(stderr, "Error: pclose() failed. %s.\n", strerror(errno));
      return EXIT_FAILURE;
    }


    return !(WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS);
}
