#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     //Sleep
#include "handling.h"

#define _GNU_SOURCE //From read line example

void menu(){
    printf("Menu\n");
    //TODO HELP MENU
}

int main(int argc, char *argv[]){
    
    // Check input parameters
    if(argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0 )) { menu() ; exit(EXIT_SUCCESS); }
    else if (argc != 5){
        fprintf(stderr, "Wrong number of arguments!\nPlease run %s -h to see properly usage\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Create local variables and copy input parameter to them
    char *serverName = parToVar(argv[1]);
    int serverPort = atoi(argv[2]);
    int receivePort = atoi(argv[3]);
    char *inputFileWithCommands = parToVar(argv[4]);

    // Juct a debug message for input parameters
    char info[1024]; 
    snprintf(info, sizeof(info), "ServerName: %s, serverPort: %d, receivePort: %d, inputFileWithCommands: %s", serverName, serverPort, receivePort, inputFileWithCommands); 
    DEBUG(info);

    // File auxiliarly variables and open file    
    FILE *fp;
    char *line = NULL ; size_t len = 0 ; ssize_t read;
    CHECKNU(fp = fopen(inputFileWithCommands, "r"));


    int cntMess = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        printf("Line:%d - %s", cntMess+1, line);

        // TODO: SENT DATA

        cntMess++;
        if(cntMess%10==0) sleep(5);
    }

    // Free resources and exit 
    fclose(fp);
    free(serverName) ; free(inputFileWithCommands) ; free(line) ;
    exit(EXIT_SUCCESS);
}