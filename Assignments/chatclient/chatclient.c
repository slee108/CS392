/*******************************************************************************
 * Name        : chatclient.c
 * Author      : Michael DelGaudio & Sunmin Lee
 * Date        : 06/30/2020
 * Description : chatclient
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/

#include <arpa/inet.h> 
#include <errno.h> 
#include <fcntl.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h>
#include <unistd.h> 
#include "./util.h"

int client_socket = -1;
char username[MAX_NAME_LEN + 1]; 
char inbuf[BUFLEN + 1];
char outbuf[MAX_MSG_LEN + 1];

int handle_stdin(int file) {
    char buf[MAX_MSG_LEN];
    int n; 
    if((n = read(file, buf, MAX_MSG_LEN)) < 0){
        fprintf(stderr, "Error: read has failed. %s.\n", strerror(errno));
        return EXIT_FAILURE;
    } else if(n == 0){
        // End of file
        return -1; 
    } else {
        //read data
        printf("%s\n", buf);
        return EXIT_SUCCESS;
    }
}

// Sends message to server
// Returns -1 if failed
int sentMessage(int sizeOfMsg, char * outbuf, int * client_socket){
    int numSent;
    if((numSent = send(*client_socket, outbuf, sizeOfMsg, 0)) < 0){
        return -1;
    }
    return numSent;
}

// Gets message from server
int handle_client_socket(char * inbuf, int * client_socket, bool inClient){
    
    int bytes_recvd = recv(*client_socket, inbuf,BUFLEN, 0);
    if(bytes_recvd == 0){
        // ALL BUSY
        if(inClient){
            fprintf(stderr, "\nConnection to server has been lost.\n");
            inbuf[bytes_recvd] = '\0';
        } else{
            fprintf(stderr, "All connections are busy. Try again later.\n");
        }
        
        return bytes_recvd;

    } else if (bytes_recvd < 0){
        // GENERAL FAIL
        if(inClient){
            fprintf(stderr, "Warning: Failed to receive incoming message. %s.\n\n", strerror(errno));
        } else{
            fprintf(stderr, "Error: Failed to receive message from server. %s.\n", strerror(errno));
        }
        
        return bytes_recvd;
    }

    //ENSURE TERMINATION
    inbuf[bytes_recvd] = '\0';
    //Send it back
    return bytes_recvd;
}


//Main runner
int main(int argc, char *argv[]) {

    //Too little args
    if (argc != 3){
        fprintf(stderr, "Usage: %s <server IP> <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    //From lab 11
    struct sockaddr_in serv_addr;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    memset(&serv_addr, 0, addrlen);

    char * userInputIP = argv[1];   //we need a way to store the userInput IP
    int userInputPort;  //way to store userInput PORT
    int returnVal = 0;

    //lab11
    //Check if valid IP
    int ip_conversion = inet_pton(AF_INET, userInputIP, &serv_addr.sin_addr);
    if (ip_conversion == 0) {
        fprintf(stderr, "Error: Invalid IP address '%s'.\n", userInputIP);
        returnVal = EXIT_FAILURE; 
        goto EXIT;
    } else if (ip_conversion < 0) {
        //this is from the lab not sure if we keep
        fprintf(stderr, "Error: Failed to convert IP address. %s.\n",
                strerror(errno));
        returnVal = EXIT_FAILURE;
        goto EXIT;
    }

    //Check valid PORT
    //is int
    if(parse_int(argv[2], &userInputPort, "port number") == false){
        return EXIT_FAILURE;
    }
    //is an actual port
    if(userInputPort > 65535 || userInputPort < 1024){
        fprintf(stderr, "Error: Port must be in range [1024, 65535].\n");
        return EXIT_FAILURE;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(userInputPort); //host to network conversion

    int stringCapture = 1;
    while(stringCapture != 0){
        printf("Enter your username: "); //what does he want it to say
        fflush(stdout);
        stringCapture = get_string(username, 21);
        if(stringCapture == 2){ //too long
            fprintf(stderr, "Sorry, limit your username to %d characters.\n", MAX_NAME_LEN);
        }
    }

    printf("Hello, %s. Let's try to connect to the server.\n", username);

    // CREATE SOCKET
    if((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        fprintf(stderr, "Error: Failed to create socket. %s.\n", strerror(errno));
        returnVal = EXIT_FAILURE;
        goto EXIT;
    }

    // CONNECT
    if (connect(client_socket, (struct sockaddr *) &serv_addr, addrlen) == -1){
        fprintf(stderr, "Error: Failed to connect to server. %s.\n", strerror(errno));
        returnVal = EXIT_FAILURE;
        goto EXIT;
    }

    // GET MESSAGE
    int getMessageVal = handle_client_socket(inbuf, &client_socket, false);
    if (getMessageVal < 1){ //failed to recieve from server
        //if it fails
        returnVal = EXIT_FAILURE;
        goto EXIT;
    }

    printf("\n%s\n\n", inbuf);

    // SEND USERNAME TO SERVER
    strcpy(outbuf, username);

    if (sentMessage(strlen(username), outbuf, &client_socket) == -1){
        fprintf(stderr, "Error: Failed to send username to server. %s.\n", strerror(errno));
        returnVal = EXIT_FAILURE;
        goto EXIT;
    }

    // username in promt like --> Mike:
    printf("[%s]: ", username);
    fflush(stdout);

    //Theres gonna have to be a while like true loop
    //where we send and recieve messages and need
    //FD_SET or FD_ISSET stuff
    //fd_set setting;
    
    fd_set fds;

    
    while(1){

        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        FD_SET(client_socket, &fds);
        //fd_set back_up = fds;
        if((select(FD_SETSIZE, &fds, NULL, NULL, NULL)) < 0){
            fprintf(stderr, "Error: select has failed. %s.\n", strerror(errno));
            returnVal = EXIT_FAILURE; 
            goto EXIT;
        } 


        if(FD_ISSET(STDIN_FILENO, &fds)){

            int got_string = get_string(outbuf, MAX_MSG_LEN + 1);
                
            if(got_string == 2){
                fprintf(stderr, "Sorry, limit your message to %d characters.\n", MAX_MSG_LEN);
                printf("[%s]: ", username);
                fflush(stdout);
            } else {

                int len = strlen(outbuf) - 1;
                if(outbuf[len] == '\n'){
                    outbuf[len] = '\0';
                }
                //send message to server
                if(send(client_socket, outbuf, strlen(outbuf), 0) < 0){
                    fprintf(stderr, "Error: Failed to send message to server. %s.\n", strerror(errno));
                    returnVal = EXIT_FAILURE; 
                    goto EXIT;
                } 
                if(strncmp(outbuf, "bye", sizeof(outbuf)) == 0){
                    printf("Goodbye.\n");
                    returnVal = EXIT_SUCCESS;
                    goto EXIT;
                } else{ 
                    printf("[%s]: ", username);
                    fflush(stdout);
                }

            }
        } 
            
        if(FD_ISSET(client_socket, &fds)){
            // client socket
            int handling = handle_client_socket(inbuf, &client_socket, true);
                
            if(handling < 0){
                //fprintf(stderr, "Warning: Failed to receive incoming message. %s.\n", strerror(errno));
                returnVal = EXIT_FAILURE;
                goto EXIT; 
            }

            if(strcmp(inbuf, "bye") == 0){
                printf("\nServer initiated shutdown.\n");
                returnVal = EXIT_SUCCESS;
                goto EXIT;
            } else if (handling == 0){
                //connection to server was lost
                returnVal = EXIT_FAILURE;
                goto EXIT; 
            } else {
                printf("\n%s\n", inbuf);
                printf("[%s]: ", username);
                fflush(stdout);
            }
        }
    }
      
EXIT:
    if (fcntl(client_socket, F_GETFD) >= 0) {
        close(client_socket);
    }
    return returnVal;

}