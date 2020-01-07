#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/wait.h>       // For sockets
#include <sys/socket.h>     // For sockets
#include <sys/types.h>      // For sockets

#include <netinet/in.h>     // Internet addresses are defined here
#include <arpa/inet.h>
#include <unistd.h>         // Fork
#include <unistd.h>         // For Sleep
#include <netdb.h>          // gethostbyaddr

#include "handling.h"

#define _GNU_SOURCE //From read line example


void menu(){
    printf("TODO\n");
}

/**
    @brief:

    @param:

    @return:
*/
int receiveFromServer(char *process, int receivePort){
    DEBUG("%s-(%s) Try to connect to Server in order to receive responses...", CLIENT, process);
}

/**
    @brief:

    @param:

    @return:
*/
int sentToServer(char *process, char *serverName, int serverPort, char *inputFileWithCommands){
    
    DEBUG("%s-(%s) Get hostname...", CLIENT, process);
    struct hostent *remote_addr; 
    if(!(remote_addr= gethostbyname(serverName))){herror ("gethostbyname"); exit(EXIT_FAILURE);}

    DEBUG("%s-(%s) Try to connect to Server in order to sent requests...", CLIENT, process);
    
    DEBUG("%s-(%s) Create Client Socket...", CLIENT, process);
    int sockfd;
    CHECKNO(sockfd=socket(AF_INET, SOCK_STREAM, 0));

    DEBUG("%s-(%s) Convert IPv4 to binary form...", CLIENT, process);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET; 
    memcpy(&serv_addr.sin_addr , remote_addr->h_addr, remote_addr->h_length ) ;
    serv_addr.sin_port = htons(serverPort);

    DEBUG("%s-(%s) Client try to connect to server...", CLIENT, process);
    CHECKNO(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)));

    DEBUG("%s-(%s) Open input file...", CLIENT, process);   
    FILE *fp;
    char *line = NULL ; size_t len = 0 ; ssize_t read;
    CHECKNU(fp = fopen(inputFileWithCommands, "r"));

    DEBUG("%s-(%s) Start sending packets...", CLIENT, process);
    int cntMess = 0;
    while ((read = getline(&line, &len, fp)) != -1) {

        // TODO: SENT DATA

        cntMess++;
        if(cntMess%10==0) sleep(5);
    }

    // Free resources and exit 
    fclose(fp);
    free(line);
}


//-------------------------------------------------------------------------
int main(int argc, char *argv[]){
    
    // Check input parameters
    if(argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0 )) {menu() ; exit(EXIT_SUCCESS);}
    else if (argc != 5){
        fprintf(stderr, "Wrong number of arguments!\nPlease run %s -h to see properly usage\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Create local variables and copy input parameter to them
    char *serverName = parToVar(argv[1]);
    int serverPort = atoi(argv[2]);
    int receivePort = atoi(argv[3]);
    char *inputFileWithCommands = parToVar(argv[4]);
    DEBUG("%s-(Input Parameters) ServerName: %s, serverPort: %d, receivePort: %d, inputFileWithCommands: %s", CLIENT, serverName, serverPort, receivePort, inputFileWithCommands);
    if (serverPort<1 || receivePort<1){
        fprintf(stderr, "Port parameters must be acceptable!\nPlease run %s -h to see properly usage\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    DEBUG("%s Create process for receiving messages...", CLIENT);
    pid_t childpid;
    CHECKNO(childpid = fork());

    if (childpid == 0){
        receiveFromServer("Child", receivePort);
    }
    else{
        sentToServer("Parent", serverName, serverPort, inputFileWithCommands);
    }

    free(serverName) ; free(inputFileWithCommands) ; 
    exit(EXIT_SUCCESS);
}
