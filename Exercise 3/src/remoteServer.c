#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "handling.h"

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
    if(argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0 )) {menu(); exit(EXIT_SUCCESS);}


    exit(EXIT_SUCCESS);
}