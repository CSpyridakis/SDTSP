#include "handling.h"

#define SERVER_BACKLOG 5

/* Wait for all dead child processes */
void sigchld_handler (int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0) ;
}

void menu(){
    printf("Usage: ./remoteServer portNumber numChildren\n");
}

int handleConnections(char *process, int client_socket){
    char buffer[BUFSIZE];
    size_t bytes_read;
    commandPackage cp;
    CHECKNO(bytes_read = recv(client_socket, &cp, sizeof(cp), 0)); 
    if(strcmp(cp.command, "EOF")==0) return 1;
    
    char add[BUFSIZE]; char com[BUFSIZE]; char comm[BUFSIZE];
    
    strcpy(add, cp.address);  strcpy(com, cp.command);
    DEBUG("%s-(%s) \t #REQUEST#  Line: [%d], Port: [%d], Addr: [%s], Command: [%s]", SERVER, process, cp.lineNumber, cp.port, add, com);
    
    commToExecute(cp.command, comm); 
    
    strcpy(cp.command, comm); strcpy(com, cp.command);
    DEBUG("%s-(%s) \t #FILTERED# Line: [%d], Port: [%d], Addr: [%s], Command: [%s]\n",SERVER, process, cp.lineNumber, cp.port, add, com)


    sendDatagram(process, comm, cp.lineNumber, "localhost", cp.port); //TODO DELETE

    LAST_REQUEST=gettime(); // TODO : timeout
    return 0;
}   


int receiveFromClient(char *process, int portNumber){
    
    char buffer[2048];

    int sockfd;
    
    // Init address struct
    struct sockaddr_in server_addr;
    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(portNumber);

    DEBUG("%s-(%s) Create Server Socket...", SERVER, process);
    CHECKNE(sockfd=socket(AF_INET, SOCK_STREAM, 0)); 

    int reuse = 1;
    DEBUG("%s-(%s) Address reuse...", SERVER, process);
    CHECKNE(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*) &reuse, sizeof(reuse)));

    DEBUG("%s-(%s) Bind Server socket...", SERVER, process);
    CHECKNE(bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)));

    DEBUG("%s-(%s) Listen socket...", SERVER, process);
    CHECKNE(listen(sockfd, SERVER_BACKLOG));

    DEBUG("%s-(%s) Socket created successfully! Socket descriptor: %d", SERVER, process, sockfd);

    int addr_size, client_socket;
    struct sockaddr_in client_addr;
    char *message = (char*) malloc(sizeof(char)*BUFSIZE);
    
    struct sockaddr_in client ;
    struct hostent *rem ;

    while (TRUE){
        DEBUG("%s-(%s) Waiting client to connect...", SERVER, process);
        addr_size = sizeof(struct sockaddr_in);
        
        CHECKNO(client_socket=accept(sockfd, (struct sockaddr*)&client_addr, (socklen_t *)&addr_size));
        DEBUG("%s-(%s) Client connected successfully!", SERVER, process);
        
        // CHECHNU(rem=gethostbyaddr((char *)&client.sin_addr.s_addr, sizeof(client.sin_addr.s_addr), client.sin_family)); 

        // printf ( "Accepted connection from %s \n", rem->h_name ) ;

        LAST_REQUEST=gettime();

        while(TRUE){
            if(timeout() || handleConnections("Parent", client_socket) == 1){
                close(client_socket);
                DEBUG("%s-(%s) Closing current connection...", SERVER, process);
                break;
            }
        }
    }
    free(message); 
}

//-------------------------------------------------------------------------
int main(int argc, char *argv[]){   
    
    // Check input parameters
    if(argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0 )) {menu() ; exit(EXIT_SUCCESS);}
    else if(argc != 3){
        fprintf(stderr, "Wrong number of arguments!\nPlease run %s -h to see properly usage\n", argv[0]);
        exit(EXIT_FAILURE);   
    }

    // Input parameters to variables 
    int portNumber = atoi(argv[1]);
    int numChildren = atoi(argv[2]);
    DEBUG("%s-(Input Parameters) portNumber: %d, numChildren: %d", SERVER, portNumber, numChildren);
    if (portNumber<1){
        fprintf(stderr, "Port parameters must be acceptable!\nPlease run %s -h to see properly usage\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    else if (numChildren<1 || numChildren>1024){  // TODO FIX IT
        fprintf(stderr, "Number of childrens must be acceptable\nPlease run %s -h to see properly usage\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Reap dead children a s y n c h r o n o u s l y */
    signal (SIGCHLD, sigchld_handler);

    // PROCESSES
    int FATHER=getpid();
    DEBUG("%s FATHER's PID %d...", SERVER, FATHER);
    pid_t childpid;
    int i;
    for (i = 0; i < numChildren ; i ++){
        // if ( ( childpid = fork () ) > 0 && FATHER==getpid() ){
        //     DEBUG ( " i : % d process ID : % d parent ID :% d child ID :% d \n " ,i , getpid () , getppid () , childpid ) ;
        //     sleep (3) ;
        // }
    }
    if (FATHER==getpid()){
        receiveFromClient("Parent", portNumber);
    }
    
    


    // CHECKNO(childpid = fork());

    // if (childpid == 0){
    //     receiveFromServer("Child");
    // }
    // else{

    // }

    sleep(10);
    exit(EXIT_SUCCESS);
}
