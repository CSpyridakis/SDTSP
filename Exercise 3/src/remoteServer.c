#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/wait.h>       // For sockets
#include <sys/socket.h>     // For sockets
#include <sys/types.h>      // For sockets

#include <netinet/in.h>     // Internet addresses are defined here

#include <arpa/inet.h>

#include <unistd.h>         // Fork

#include "handling.h"

#define SERVER_BACKLOG 5


void menu(){
    printf("TODO\n");
}

void handleConnections(int client_socket){
    char buffer[BUFSIZE];
    size_t bytes_read;
    int messageSize = 0;

    CHECKNO(bytes_read = read(client_socket, buffer, BUFSIZE)); 

    DEBUG("REQUEST: %s", buffer);
    close(client_socket);
    DEBUG("Closing connection");
}   

// TODO in CTR + C signal in client send end to Server

int receiveFromClient(char *process, int portNumber){
    
    int sockfd;
    
    // Init address struct
    struct sockaddr_in server_addr;
    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(portNumber);

    DEBUG("[%s] Create Server Socket...", process);
    CHECKNO(sockfd=socket(AF_INET, SOCK_STREAM, 0)); 

    DEBUG("[%s] Bind Server socket...", process);
    CHECKNO(bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)));

    DEBUG("[%s] Listen socket...", process);
    CHECKNO(listen(sockfd, SERVER_BACKLOG));

    DEBUG("[%s] Socket created successfully! Socket descriptor: %d", process, sockfd);

    int addr_size, client_socket;
    struct sockaddr_in client_addr;
    while (1){
        DEBUG("[%s] Waiting client to connect...", process);
        addr_size = sizeof(struct sockaddr_in);
        
        CHECKNO(client_socket=accept(sockfd, (struct sockaddr*)&client_addr, (socklen_t *)&addr_size));
        DEBUG("[%s] Client Connected", process);

        handleConnections(client_socket);
    }
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
    if (portNumber<1){
        fprintf(stderr, "Port parameters must be acceptable!\nPlease run %s -h to see properly usage\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    else if (numChildren<1 || numChildren>1024){  // TODO FIX IT
        fprintf(stderr, "Number of childrens must be acceptable\nPlease run %s -h to see properly usage\n", argv[0]);
        exit(EXIT_FAILURE);
    }

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
        receiveFromClient("Parent", portNumber);
    // }

    sleep(10);
    exit(EXIT_SUCCESS);
}