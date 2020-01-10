#include <stdio.h> 
#include <stdlib.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <netdb.h>          // gethostbyaddr
#include "handling.h"
#define PORT 8080 

// int main(int argc, char const *argv[]){
//     if(argc != 3){
//         fprintf(stderr, "Wrong number of arguments!\nPlease run %s -h to see properly usage\n", argv[0]);
//         exit(EXIT_FAILURE);   
//     }
//     int numOfkids = atoi(argv[1]);
//     char *command = parToVar(argv[2]);

//     int i;
//     for(i=0;i<5;i++){
//         printf("%s\n", ACCEPTABLE_COMMANDS[i]);
//     }
// }



int main(int argc, char const *argv[])
{
    // if(argc != 2){
    //     fprintf(stderr, "Wrong number of arguments!\nPlease run %s -h to see properly usage\n", argv[0]);
    //     exit(EXIT_FAILURE);   
    // }
    // int numOfkids = atoi(argv[1]);
    char *str = parToVar(argv[1]);

    printf("Input Line: %s\n", str);
    findValidCommands(str);

	





    



    // char delim2[] = "|";
	// while(ptr != NULL)
	// {
	// 	printf("'%s'\n", ptr);
	// 	ptr = strtok(str, delim2);
	// }

	// /* This loop will show that there are zeroes in the str after tokenizing */
	// for (int i = 0; i < init_size; i++)
	// {
	// 	printf("%d ", str[i]); /* Convert the character to integer, in this case
	// 						   the character's ASCII equivalent */
	// }
	// printf("\n");

	return 0;
}