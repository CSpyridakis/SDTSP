#ifndef HANDLING_H
#define HANDLING_H

// Change DEBUG_S to 1 to enable debug messages and to 0 to disable them 
#define DEBUG_S 1

#include <stdio.h>
#include <errno.h> 
#include <string.h>
#include <stdlib.h>

#define BUFSIZE 4096

#define RED     "\033[31;1m"
#define GREEN   "\033[32;1m"
#define YELLOW  "\033[33;1m"
#define BLUE    "\033[34;1m"
#define PURPLE  "\033[35;1m"
#define LBLUE   "\033[36;1m"
#define WHITE   "\033[37;1m"
#define NORMAL  "\033[0m"

#define CLIENT  "[\033[35;1mCLIENT\033[37;1m]"
#define SERVER  "[\033[36;1mSERVER\033[37;1m]"

// For debug messages constraction
char infoBuffer[BUFSIZE];

// Print kind of message (DEBUG, ERROR) and actual text in stderr 
#define PRINTMESS(kindof, mess) {fprintf(stderr, "[ %s ] %s{%s:%d}-(%s:%d)%s %s %s\n", kindof, YELLOW, __FILE__, __LINE__, __FUNCTION__, getpid(), WHITE, (mess), NORMAL);}

// Create debug message, print it in stderr and empty infoBuffer
#define DEBUG(mess...) {if (DEBUG_S) {snprintf(infoBuffer, sizeof(infoBuffer), mess); PRINTMESS(COLOR("DEBUG", GREEN), infoBuffer); strcpy(infoBuffer, "");}}

// Print error message and abort
#define FATAL(mess) {PRINTMESS(COLOR("ERROR", RED), mess); abort();}

// Find out actual error based on errcode
#define FATALERR(errcode) FATAL(strerror(errcode))


// -----------------------------------------------------
// CHECK MACROS 
#define CHECKNO(X) {if ((X) == (-1)) FATALERR(errno);}

#define CHECKNU(X) {if ((X) == NULL) FATALERR(errno);}

#define CHECKNE(X) {if ((X) < 0) FATALERR(errno);}


/**
    @brief: Change color of a given message

    @param: TEXT the text that you want to change its color.
            It must not be more than 200 characters.
    
    @param: COL the actual color you want TEXT has.

    @see: Object-like Macros in the top of this header file
            for available colors.

    @return: 
*/
char * COLOR(char * TEXT, char * COL){
    static char buf[256];
    snprintf(buf, sizeof(buf), "%s%s%s", COL, TEXT, NORMAL);
    return buf;
}


/**
    @brief: Take a program's input parameter that is text and converts it
            in variable (actualy a pointer in char) 
            

    @param: par the parameter that you want to read

    @return:  parToVar a pointer to the needed string
*/
char *parToVar(char *par){
    int len = strlen(par);
    char *var;
    CHECKNU(var=(char*)malloc(len*sizeof(char)));
    strcpy(var, par);
    return var;
}

#endif //HANDLING_H
