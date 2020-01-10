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


#define REG_END "end"
#define REG_CLO "timeToStop"
#define REG_LIN "^[ ]*[ls|cat]"


#define MORE_THAN_100_CHARS -1


int REGEX_SIZE=4096;
char REGEX[4096];

#define SPACE_NN      "[ ]*"
#define SPACE_ME      "[ ]+"
#define END_COMM      "end"
#define STOP_COMM     "timeToStop"
#define SIMPLE_COMM   "(ls|cat|cut|tr|grep|" END_COMM "|" STOP_COMM ")"	

// COMMAND parameters
#define DUAL_QUAT     "(\\\"[^\\\"]*\\\")"
#define SING_QUAT     "(\\\'[^\\\']*\\\')"
#define QUATS         "(" DUAL_QUAT "|" SING_QUAT ")"
#define FLAG          "-[\\\'\\\"]*" QUATS "?"

#define ELSE_PARAM    "([^\\\"\\\']*)"

#define PARAM_TYPE    "(" DUAL_QUAT "|" SING_QUAT "|" FLAG "|" ELSE_PARAM ")"
#define PARAM         "(" PARAM_TYPE SPACE_ME ")+"

// COMMAND formats
#define COMMAND_NP    SPACE_NN SIMPLE_COMM SPACE_NN
#define COMMAND_WP    SPACE_NN SIMPLE_COMM SPACE_ME PARAM

// COMMAND
#define COMMAND       "(" COMMAND_NP "|" COMMAND_WP ")"

// VALID LINE
#define VALID_LINE    "^(" COMMAND "|)*" COMMAND "$"


int main(int argc, char const *argv[]){
	printf("^%s$\n", FLAG);	
	return 0;
}
















// int lineParse(const char* string){
	
// 	if (sizeof(string)>100){
// 		return MORE_THAN_100_CHARS;
// 	}




// }

// if(argc != 2){
    //     fprintf(stderr, "Wrong number of arguments!\nPlease run %s -h to see properly usage\n", argv[0]);
    //     exit(EXIT_FAILURE);   
    // }
    // int numOfkids = atoi(argv[1]);
    // char *str = parToVar(argv[1]);

    // system(str);
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
