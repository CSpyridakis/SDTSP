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

char *parToVar(char *par){
    int len = strlen(par);
    char *var;
    CHECKNU(var=(char*)malloc(len*sizeof(char)));
    strcpy(var, par);
    return var;
}

int main(int argc, char *argv[]){
    
    // Check input parameters
    if(argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0 )) menu();
    else if (argc != 5){
        printf("Wrong number of arguments!\nPlease run %s -h to see properly usage\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Create local variables and copy input parameter to them
    char *serverName = parToVar(argv[1]);
    char *serverPort = parToVar(argv[2]);
    char *receivePort = parToVar(argv[3]);
    char *inputFileWithCommands = parToVar(argv[4]);

    // printf("Server Name: %s, Server port: %s, Receive port: %s, Inputfile: %s\n", serverName, serverPort, receivePort, inputFileWithCommands);

    // File auxiliarly variables and open file    
    FILE *fp;
    char *line = NULL ; size_t len = 0 ; ssize_t read;
    CHECKNU(fp = fopen(inputFileWithCommands, "r"));

    int cnt = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        printf("Line:%d - %s", cnt+1, line);

        // TODO: SENT DATA

        cnt++;
        if(cnt%10==0) sleep(5);
    }

    // Free resources and exit 
    fclose(fp);
    free(serverName) ; free(serverPort) ; free(receivePort) ; free(inputFileWithCommands) ; free(line) ;
    exit(EXIT_SUCCESS);
}