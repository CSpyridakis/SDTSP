#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>     // For sockets
#include <sys/types.h>

#include <netinet/in.h>     // Internet addresses are defined here

#include <arpa/inet.h>

#include <unistd.h>         // Fork

#include "handling.h"


void menu(){
    printf("TODO\n");
}

// TODO in CTR + C signal in client send end to Server

int receiveFromClient(char *process, int portNumber){

    DEBUG("[%s] Create Server Socket...", process);

    // Create a Socket for Receiving data from client
    int sockfd;
    CHECKNO(sockfd=socket(AF_INET, SOCK_STREAM, 0)); 

    DEBUG("Socket created successfully! Socket descriptor: %d", sockfd);
}

//-------------------------------------------------------------------------
int main(int argc, char *argv[]){   
    
    // Check input parameters
    if(argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0 )) {menu() ; exit(EXIT_SUCCESS);}
    else if(argc != 3){
        fprintf(stderr, "Wrong number of arguments!\nPlease run %s -h to see properly usage\n", argv[0]);
        exit(EXIT_FAILURE);   
    }

    // Input parameters to variables 
    int portNumber = atoi(argv[1]);
    int numChildren = atoi(argv[2]);
    DEBUG("[Input Parameters] portNumber: %d, numChildren: %d", portNumber, numChildren);


    // PROCESSES 
    // pid_t childpid;
    // int i;
    // for (i = 1; i < numChildren ; i ++)
    //     if ( ( childpid = fork () ) == 0 )
    //         break;

    // printf ( " i : % d process ID : % d parent ID :% d child ID :% d \n " ,i , getpid () , getppid () , childpid ) ;
    // sleep (1) ;


    // CHECKNO(childpid = fork());

    // if (childpid == 0){
    //     receiveFromServer("Child");
    // }
    // else{
        // receiveFromClient("Parent", portNumber);
    // }

    sleep(10);
    exit(EXIT_SUCCESS);
}