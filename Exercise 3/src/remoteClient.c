#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>     // For sockets
#include <sys/types.h>

#include <netinet/in.h>     // Internet addresses are defined here

#include <arpa/inet.h>

#include <unistd.h>         // Fork

#include <unistd.h>         // For Sleep

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
    DEBUG("[%s] Try to connect to Server...", process);
}

/**
    @brief:

    @param:

    @return:
*/
int sentToServer(char *process, char *serverName, int serverPort, char *inputFileWithCommands){
    // File auxiliary variables and open file    
    FILE *fp;
    char *line = NULL ; size_t len = 0 ; ssize_t read;
    CHECKNU(fp = fopen(inputFileWithCommands, "r"));

    DEBUG("[%s] Try to connect to Server...", process);

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
    DEBUG("[Input Parameters] ServerName: %s, serverPort: %d, receivePort: %d, inputFileWithCommands: %s", serverName, serverPort, receivePort, inputFileWithCommands);

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