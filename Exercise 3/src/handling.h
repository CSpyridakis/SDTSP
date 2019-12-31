#ifndef HANDLING_H
#define HANDLING_H

// Change DEBUG_S to 1 to enable debug messages and to 0 to disable them 
#define DEBUG_S 1

#include <stdio.h>
#include <errno.h> 
#include <string.h>
#include <stdlib.h> // Exit_success and exit_failure  exit(EXIT_FAILURE);


#define BLACK   "\033[30;7m"
#define RED     "\033[31;1m"
#define GREEN   "\033[32;1m"
#define YELLOW  "\033[33;1m"
#define WHITE   "\033[37;1m"
#define NORMAL  "\033[0m"


#define PRINTMESS(kindof, mess) { fprintf(stderr, "[%s] %s{%s:%d}-(%s)%s %s\n", kindof, YELLOW, __FILE__, __LINE__, __FUNCTION__, NORMAL, (mess)); }

#define DEBUG(mess) { if (DEBUG_S) {PRINTMESS(COLOR("DEBUG", GREEN), mess); strcpy(mess, "");}}

#define FATAL(mess) { PRINTMESS(COLOR("ERROR", RED), mess) ; abort(); }

#define FATALERR(errcode) FATAL(strerror(errcode))

// CHECK MACROS
#define CHECKNO(X) {if ((X) == -1) FATALERR(errno);}

#define CHECKNU(X) {if ((X) == NULL) FATALERR(errno);}

#define CHECKNE(X) {if ((X) < 0) FATALERR(errno);}

#define CHECK(X) {if ((X) == NULL || (X) != 0) FATALERR(errno);}


/**
    @brief:

    @param:

    @return:
*/
char * COLOR(char * TEXT, char * COL){
    static char buf[256];
    snprintf(buf, sizeof(buf), "%s%s%s", COL, TEXT, NORMAL);
    return buf;
}


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