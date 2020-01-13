#include <stdio.h> 
#include <stdlib.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <netdb.h>          // gethostbyaddr
#include "handling.h"
#define PORT 8080 
   
int main(int argc, char const *argv[]) { 
    struct hostent *mymachine = gethostbyname(argv[1]);
    int i;
    struct in_addr **addr_list;
    char hostname [50] , symbolicip [50];
    if(!mymachine){herror ("gethostbyname"); exit(EXIT_FAILURE);}
    else{
        printf ("Name To Be Resolved: %s \n" ,mymachine->h_name);
        printf ("Name Length in Bytes: %d \n" ,mymachine->h_length);
        addr_list=(struct in_addr **) mymachine->h_addr_list ;
        for (i= 0; addr_list[i] != NULL ; i ++) {
            strcpy (symbolicip, inet_ntoa(* addr_list[i]));
            printf ("%s resolved to %s \n ", mymachine->h_name, symbolicip );
        }
    }
} 