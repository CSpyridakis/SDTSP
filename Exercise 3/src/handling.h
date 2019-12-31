#ifndef HANDLING_H
#define HANDLING_H

// Change DEBUG_S to 1 to enable debug messages and to 0 to disable them 
#define DEBUG_S 1

#include <stdio.h>
#include <errno.h> 
#include <string.h>
#include <stdlib.h> // Exit_success and exit_failure  exit(EXIT_FAILURE);

#include <stdarg.h> // For unknown number of arguments

#define PRINTMESS(kindof, mess) { fprintf(stderr, "[%s] {%s:%d}-(%s) %s\n", kindof, __FILE__, __LINE__, __FUNCTION__, (mess)); }

#define DEBUG(mess) { if (DEBUG_S) {PRINTMESS("DEBUG", mess); strcpy(mess, "");}}

#define FATAL(mess) { PRINTMESS("ERROR", mess) ; abort(); }

#define FATALERR(errcode) FATAL(strerror(errcode))

// CHECK MACROS
#define CHECKNO(X) {if ((X) == -1) FATALERR(errno);}

#define CHECKNU(X) {if ((X) == NULL) FATALERR(errno);}

#define CHECKNE(X) {if ((X) < 0) FATALERR(errno);}

#define CHECK(X) {if ((X) == NULL || (X) != 0) FATALERR(errno);}

// TODO
// char * COLOR(char * TEXT, char * COL){
//     int len = 
//     return snprintf(info, sizeof(info), "ServerName: %s, serverPort: %d, receivePort: %d, inputFileWithCommands: %s", serverName, serverPort, receivePort, inputFileWithCommands); 
// }


/**
    @brief:

    @param:

    @return:
*/
char *parToVar(char *par){
    int len = strlen(par);
    char *var;
    CHECKNU(var=(char*)malloc(len*sizeof(char)));
    strcpy(var, par);
    return var;
}

#endif //HANDLING_H