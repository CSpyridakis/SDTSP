#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

int main(int argc , char *argv[]){
    pid_t childpid;
    pid_t mypid;
    int i,n;
    
    if (argc!=2){
        printf("Usage: %s number -of-processes \n",argv [0]);
        exit (1);
    }
    
    printf("The id of the initial process is %d\n",getpid());
    n = atoi(argv[1]);
    
    for (i=1;i<n; i++)
        if ( (childpid = fork()) == 0 )
            break ;
    
    printf("i: %d process ID: %d parent ID:%d child ID:%d\n", i, getpid(), getppid(), childpid);
    
    sleep(1);
    
    return EXIT_SUCCESS;
}