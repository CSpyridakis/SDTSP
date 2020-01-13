#ifndef HANDLING_H
#define HANDLING_H

// Basic
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h> 

// For sockets
#include <sys/wait.h>       
#include <sys/socket.h>     
     
#include <time.h>
#include <sys/time.h>

#include <netinet/in.h>     // Internet addresses are defined here
#include <arpa/inet.h>

#include <unistd.h>         // Fork
#include <netdb.h>          // gethostbyaddr

// signals
#include <signal.h>         
#include <sys/types.h>  

#define bool int
#define TRUE 1
#define FALSE 0

// Change DEBUG_S to TRUE to enable debug messages and to FALSE to disable them 
#define DEBUG_S TRUE

// Buffer size for messages and more
#define BUFSIZE 2048

// From read line example
#ifndef _GNU_SOURCE
    #define _GNU_SOURCE        
#endif
 



// _________________________________________________________________________________________
//    _____                         _                     _ _ _             
//   | ____|_ __ _ __ ___  _ __    | |__   __ _ _ __   __| | (_)_ __   __ _ 
//   |  _| | '__| '__/ _ \| '__|   | '_ \ / _` | '_ \ / _` | | | '_ \ / _` |
//   | |___| |  | | | (_) | |      | | | | (_| | | | | (_| | | | | | | (_| |
//   |_____|_|  |_|  \___/|_|      |_| |_|\__,_|_| |_|\__,_|_|_|_| |_|\__, |
//                                                                    |___/ 

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

    @return: a pointer to the needed string
*/
char *COLOR(char *TEXT, char *COL){
    static char buf[256];
    snprintf(buf, sizeof(buf), "%s%s%s", COL, TEXT, NORMAL);
    return buf;
}





// _________________________________________________________________________________________
//      _              _ _ _                  
//     / \  _   ___  _(_) (_) __ _ _ __ _   _ 
//    / _ \| | | \ \/ / | | |/ _` | '__| | | |
//   / ___ \ |_| |>  <| | | | (_| | |  | |_| |
//  /_/   \_\__,_/_/\_\_|_|_|\__,_|_|   \__, |
//                                      |___/ 

#define ALIVE 1
#define DEAD  0

/**
    @brief: Take a program's string input parameter and converts it
            to a char * variable
            
    @param: par the parameter that you want to read

    @return:  parToVar a pointer to the needed string
*/
char *parToVar(const char *par){
    int len = strlen(par);
    char *var;
    CHECKNU(var=(char*)malloc(len*sizeof(char)));
    strcpy(var, par);
    return var;
}


/**
    @brief:

    @param:

    @return:
*/
double gettime() {
    struct timeval ttime;
    gettimeofday(&ttime, NULL);
    return ttime.tv_sec + ttime.tv_usec * 0.000001;
}


/**
    @brief:

    @param:

    @return:
*/
int writeToFile(char * process, const int port, const int line, char * text){
    char filename[BUFSIZE];
    snprintf(filename, sizeof(filename), "output.%d.%d", port, line);
    // DEBUG("%s-(%s) \t #APPEND TO FILE# Line: [%d] File to append: [%s] Response: [%s]", CLIENT, process, line, filename, text);

    FILE *fPtr = fopen(filename, "a");
    if(!fPtr){
        DEBUG("%s-(%s) \t \033[31;1m[ERROR]\033[37;1m In opening file for appending", CLIENT, process);
        return -1;
    }
    fputs(text, fPtr);
    fclose(fPtr);
    return 0;
}

// _________________________________________________________________________________________
//   ____  _                   _     
//  / ___|(_) __ _ _ __   __ _| |___ 
//  \___ \| |/ _` | '_ \ / _` | / __|
//   ___) | | (_| | | | | (_| | \__ \
//  |____/|_|\__, |_| |_|\__,_|_|___/
//           |___/    
#define SIGEND SIGUSR1
#define SIGTTS SIGUSR2



// void endChild(int sig, siginfo_t *info, void *context){
//     signal(SIGEND, endChild); 

//     fprintf(stderr, "End\n");
// }

// void timeToStop(int sig, siginfo_t *info, void *context){
//     signal(SIGTTS, timeToStop);
//     fprintf(stderr, "TimeToStop\n");
// }


// _________________________________________________________________________________________
//   ____             _        _       
//  / ___|  ___   ___| | _____| |_ ___ 
//  \___ \ / _ \ / __| |/ / _ \ __/ __|
//   ___) | (_) | (__|   <  __/ |_\__ \
//  |____/ \___/ \___|_|\_\___|\__|___/

#define READ 0
#define WRITE 1

#define SLEEP 1

double TIMEOUT=10;

// Package format to send on TCP
typedef struct address { 
   char command[1024]; 
   char address[1024];
   int port;
   int lineNumber; 
} commandPackage;

typedef struct handler { 
   commandPackage cp; 
   struct sockaddr_in client_addr; 
} handlePackage;

#define TEXT_RESPONSE_LEN 450 
typedef struct response { 
   char response[TEXT_RESPONSE_LEN]; 
   int lineNumber; 
} responsePackage;


double LAST_REQUEST;

bool timeout(){
    double now=gettime();
    if(now-LAST_REQUEST>=TIMEOUT){
       return TRUE; 
    }
    return FALSE;
}


void sendDatagram(char * process, const char *message, const int line, struct sockaddr_in server_addr, const int port){
    
    // Create response address
    struct hostent *remote_addr;
    if(!(remote_addr=gethostbyaddr((const char *)&server_addr.sin_addr.s_addr,sizeof(server_addr.sin_addr.s_addr),AF_INET))){FATAL("ERROR with hostname!");return;}
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = *(long *)(remote_addr->h_addr_list[0]);
    
    // DEBUG("%s-(%s) ~~~ Create ~~~ response socket...", SERVER, process);
    int sockfd;
    CHECKNO(sockfd=socket(AF_INET, SOCK_DGRAM, 0));
    
    responsePackage rp;
    strcpy(rp.response, message);
    rp.lineNumber=line;

    char ip[30];
    strcpy(ip, (char*)inet_ntoa((struct in_addr)serv_addr.sin_addr));
    
    if(sendto(sockfd, &rp, sizeof(rp)+1, 0, (struct sockaddr *)&serv_addr, sizeof (serv_addr)) < 0){
        DEBUG("%s-(%s)    \033[31;1m[ERROR]\033[37;1m In sending datagram. Package Line: [%d], IP: [%s], Port: [%d], Text: [%s]", SERVER, process, rp.lineNumber, ip, server_addr.sin_port, rp.response);
    }
    else{
        DEBUG("%s-(%s)    \033[33;1m[Response]\033[37;1m Package Line: [%d], IP: [%s], Port: [%d], Text: [%s]", SERVER, process, rp.lineNumber, ip, server_addr.sin_port, rp.response);
    }
    
    close(sockfd);
}

int response(handlePackage hp, char * text){
    sendDatagram("Child", text, hp.cp.lineNumber, hp.client_addr, hp.cp.port);
} 

// _________________________________________________________________________________________
//    ____                                          _       ____    
//   / ___|___  _ __ ___  _ __ ___   __ _ _ __   __| |     |  _ \ __ _ _ __ ___  ___ _ __ 
//  | |   / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` |     | |_) / _` | '__/ __|/ _ \ '__|
//  | |__| (_) | | | | | | | | | | | (_| | | | | (_| |     |  __/ (_| | |  \__ \  __/ | 
//   \____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|     |_|   \__,_|_|  |___/\___|_|
//                                                      

#define ACC_COMMANDS_NO 5
char ACCEPTABLE_COMMANDS[ACC_COMMANDS_NO][5] = {"ls", "grep", "cut", "cat", "tr"};

/**
    @brief: Check if the given command is valid. Actually check is exists in ACCEPTABLE_COMMANDS array.

    @param: command is a string containing a word that need to be checked 
            if is a supported command

    @return: TRUE if is acceptable or else FALSE 
*/
bool acceptableCommand(const char* command){
    int i;
    for (i=0;i<ACC_COMMANDS_NO;i++)
        if(strncmp(ACCEPTABLE_COMMANDS[i], command, strlen(ACCEPTABLE_COMMANDS[i])) == 0)
            return TRUE;
        
    return FALSE;
}

/**
    @brief: Find next 

    @param:

    @return:
*/
int findNextCommand(int *start, const char* str){
    int curPos = *start;
    bool started = FALSE;
    while(strlen(str) - curPos > 0){
        if(started == FALSE && str[curPos] != ' ') {
            *start = curPos;
            started = TRUE;
        }
        else if (started == TRUE && curPos+1 >= strlen(str)) return curPos;
        else if (started == TRUE && str[curPos] == ' ') return curPos-1;
        curPos++;
    }
    return strlen(str)-1;
}

/**
    @brief:

    @param:

    @return:
*/
int parseStr(int *start, const char* str){
    int curPos = *start;
    bool started = FALSE;
    char strSymbol;
    while(strlen(str) - curPos > 0){
        if(str[curPos] == '\\' && curPos+1 >= strlen(str)) 
            return strlen(str)-1; 
        else if(str[curPos] == '\\' && (str[curPos+1] == '"' || str[curPos+1] == '\'')) 
            curPos++;
        else if (started == TRUE && str[curPos] == strSymbol) 
            return curPos;
        else if(started == FALSE && (str[curPos] == '\'' || str[curPos] == '"')){
            *start=curPos; 
            started=TRUE;
            strSymbol=str[curPos];
        }
        curPos++;
    }
    return strlen(str)-1;
}

/**
    @brief:

    @param:

    @return:
*/
int parseLine(const char* str){
    int  retPos=0, next, curPos=0, tmpPos=curPos;
    bool commParsed = FALSE;
    char curCommand[2000];

    if (strlen(str)>100) return -1;

    while(strlen(str) - curPos > 0){
        if (commParsed == FALSE) {
            commParsed = TRUE;
            tmpPos=curPos;
            next = findNextCommand(&tmpPos, str);
            strncpy (curCommand, str + tmpPos, next - tmpPos + 1);
            curCommand[next - tmpPos + 1]='\0';

            if (strcmp(curCommand, "end") == 0 || strcmp(curCommand, "end ") == 0) return -2;
            else if (strcmp(curCommand, "timeToStop") == 0 || strcmp(curCommand, "timeToStop ") == 0) return -3;
            else if (acceptableCommand(curCommand) == FALSE) return retPos;

            curPos = next;
            retPos = next;
        }
        else if (commParsed == TRUE && str[curPos] == ' ') {;}
        else if (commParsed == TRUE && str[curPos] == '"' || str[curPos] == '\'') {
            tmpPos=curPos;
            next = parseStr(&tmpPos, str);
            curPos = next;
            retPos = next;
        }
        else if (commParsed == TRUE && str[curPos] == ';') return retPos;
        else if (commParsed == TRUE && str[curPos] == '|') {
            commParsed = FALSE;
        }
        else{
            retPos=curPos;
        }
        curPos++;
    }
    return retPos;
}

/**
    @brief:

    @param:

    @return:
*/
int commToExecute(const char * line, char *command){
    int ret=0;
    int i=parseLine(line);
    if (i>0){
        strncpy (command, line , i+1);
        command[i+1]='\0';
    }
    else if (i==0)  {strcpy(command, "");ret=-1;}
    else if (i==-1) {strcpy(command, "");ret=-1;}
    else if (i==-2) {strcpy(command, "end");ret=-2;}
    else if (i==-3) {strcpy(command, "timeToStop");ret=-3;}
    return ret;
}

#endif //HANDLING_H
