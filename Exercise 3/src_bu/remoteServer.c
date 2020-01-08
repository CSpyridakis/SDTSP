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

double LAST_REQUEST;

int timeout(){
    double now=gettime();
    // DEBUG("%f", now-LAST_REQUEST);
    if(now-LAST_REQUEST>=TIMEOUT){
       return 1; 
    }
    return 0;
}

void menu(){
    printf("Usage: ./remoteServer portNumber numChildren\n");
}

char *handleConnection(char *process, int client_socket){
    static char buffer[BUFSIZE];
    size_t bytes_read;
    int messageSize = 0;

    CHECKNE(bytes_read = recv(client_socket, buffer, BUFSIZE, 0)); 
    if(!strcmp(buffer, "EOF")) return NULL;
    
    return buffer;
}   

// TODO in CTR + C signal in client send end to Server

int receiveFromClient(char *process, int portNumber){
    
    char buffer[2048];

    int sockfd;
    
    // Init address struct
    struct sockaddr_in server_addr;
    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(portNumber);

    DEBUG("%s-(%s) Create Server Socket...", SERVER, process);
    CHECKNO(sockfd=socket(AF_INET, SOCK_STREAM, 0)); 

    int option = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    DEBUG("%s-(%s) Bind Server socket...", SERVER, process);
    CHECKNO(bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)));

    DEBUG("%s-(%s) Listen socket...", SERVER, process);
    CHECKNO(listen(sockfd, SERVER_BACKLOG));

    DEBUG("%s-(%s) Socket created successfully! Socket descriptor: %d", SERVER, process, sockfd);

    int addr_size, client_socket;
    struct sockaddr_in client_addr;
    char *message = (char*) malloc(sizeof(char)*BUFSIZE);
    LAST_REQUEST=gettime();

    while (1){
        DEBUG("%s-(%s) Waiting client to connect...", SERVER, process);
        addr_size = sizeof(struct sockaddr_in);
        
        CHECKNO(client_socket=accept(sockfd, (struct sockaddr*)&client_addr, (socklen_t *)&addr_size));
        DEBUG("%s-(%s) Client connected successfully!", SERVER, process);

        while(1){
            message = handleConnection("Parent", client_socket);
            if (timeout() || !message || strcmp(buffer, "CLIENT_DISCONNECTED")==0) break;
            else{
                DEBUG("%s-(%s) {REQUEST}: %s", SERVER, process, message);
                strcpy(message, "CLIENT_DISCONNECTED");
            }
        }
        close(client_socket);
        DEBUG("%s-(%s) Closing current connection...", SERVER, process);
        strcpy(message, "");
    }
    free(message);
    close(client_socket);
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
    DEBUG("%s-(Input Parameters) portNumber: %d, numChildren: %d", SERVER, portNumber, numChildren);
    if (portNumber<1){
        fprintf(stderr, "Port parameters must be acceptable!\nPlease run %s -h to see properly usage\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    else if (numChildren<1 || numChildren>1024){  // TODO FIX IT
        fprintf(stderr, "Number of childrens must be acceptable\nPlease run %s -h to see properly usage\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // PROCESSES
    int FATHER=getpid();
    DEBUG("%s FATHER's PID %d", SERVER, FATHER);

    pid_t childpid;
    int i;
    for (i = 1; i <= numChildren ; i ++){
        if ( ( childpid = fork () ) > 0 && FATHER==getpid() ){
            DEBUG ( "i:%d process ID: %d parent ID: %d child ID: %d n ", i, getpid(), getppid(), childpid) ;
            // sleep (3) ;
        }
    }
    if (FATHER==getpid()){
        receiveFromClient("Parent", portNumber);
    }
    
    sleep(10);
    exit(EXIT_SUCCESS);
}
