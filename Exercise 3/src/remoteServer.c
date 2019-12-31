#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>     // For sockets
#include <sys/types.h>

#include <netinet/in.h>     // internet addressed are defined here

#include <arpa/inet.h>

#include "handling.h"

// For debug messages
char infoBuffer[1024] ;

void menu(){
    printf("Menu\n");
    //TODO HELP MENU
}

int main(int argc, char *argv[]){
    
    // Check input parameters
    if(argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0 )) { menu() ; exit(EXIT_SUCCESS); }
    else if(argc != 3){
        fprintf(stderr, "Wrong number of arguments!\nPlease run %s -h to see properly usage\n", argv[0]);
        exit(EXIT_FAILURE);   
    }

    // Input parameters to variables 
    int portNumber = atoi(argv[1]);
    int numChildren = atoi(argv[2]);

    // Juct create and print a debug message
    snprintf(infoBuffer, sizeof(infoBuffer), "portNumber: %d, numChildren: %d", portNumber, numChildren) ; DEBUG(infoBuffer);

    // Create Socket
    int sockfd;
    CHECKNO(sockfd=socket(AF_INET , SOCK_STREAM , 0)); 

    // Juct create and print a debug message
    snprintf(infoBuffer, sizeof(infoBuffer), "Socket created successfully! Socket descriptor: %d", sockfd) ;  DEBUG(infoBuffer);


    exit(EXIT_SUCCESS);
}