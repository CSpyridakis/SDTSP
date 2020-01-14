#include "handling.h"

#define SERVER_BACKLOG 5
double LAST_REQUEST;

void menu();
void sigchld_handler(int sig);
void runServer(int numChildren, int portNumber);
void runChild(int pip[2]);
void runParent( fd_set activefds, fd_set readfds, int pip[2], int mstsockfd, struct sockaddr_in client_addr);
int readFromConnection(int client_socket,int pip[2], struct sockaddr_in client_addr);
int commandExecution(handlePackage hp);
void suicide();
bool alive();


// Children PIDs, Status and number of alive kids
volatile int CPID[1024];
volatile int KIDS_NUM;
volatile int ALIVE_KIDS;
volatile bool EXIT=FALSE;

void endChild(){
    // sleep(1);
    signal(SIGEND, endChild); 
    ALIVE_KIDS--;
    DEBUG("PARENT -KILL: %d", getpid());
    if (!alive())
        kill(getpid(), SIGINT);
}

void timeToStop(){
    int i;
    char mess[200];   
    for(i=0;i<KIDS_NUM; i++)
        kill(CPID[i], SIGINT);
    
    kill(getpid(), SIGINT);
    ALIVE_KIDS=0;
}

bool alive(){
    // sleep(1);
    int i;
    int alive=ALIVE_KIDS==0?TRUE:FALSE;
    for(i=0;i<KIDS_NUM; i++){
        if(kill(CPID[i], 0) == 0){
            return alive = TRUE;
        }
    }
    return alive;
}

void suicide(){
    char mess[200];
    snprintf(mess, sizeof(mess), "\033[31;1m[KILL]\033[37;1m Process with PID: [%d] suicide.", getpid());
    PRINTMESS(COLOR("KILL",YELLOW), mess);
    exit(EXIT_SUCCESS);
}


//-------------------------------------------------------------------------
int main(int argc, char *argv[]){
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, suicide);
    // Check input parameters
    if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)){
        menu();
        exit(EXIT_SUCCESS);
    }
    else if (argc != 3){
        fprintf(stderr, "Wrong number of arguments!\nPlease run %s -h to see properly usage\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Input parameters to variables
    int portNumber = atoi(argv[1]);
    int numChildren = atoi(argv[2]);
    DEBUG("%s-(Input Parameters) portNumber: %d, numChildren: %d", SERVER, portNumber, numChildren);
    if (portNumber < 1){
        fprintf(stderr, "Port parameters must be acceptable!\nPlease run %s -h to see properly usage\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    else if (numChildren < 1 || numChildren > 1024){ // TODO FIX IT
        fprintf(stderr, "Number of childrens must be acceptable\nPlease run %s -h to see properly usage\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Reap dead children a s y n c h r o n o u s l y */
    signal(SIGCHLD, sigchld_handler);
    runServer(numChildren, portNumber);
    exit(EXIT_SUCCESS);
}

void runServer(int numChildren, int portNumber){
    int mstsockfd, pip[2], i, reuse = 1;
    int FATHER = getpid();
    pid_t childpid;
    
    DEBUG("%s FATHER's PID %d...", SERVER, FATHER);

    // PIPE creation
    CHECKNO(pipe(pip));
    KIDS_NUM = numChildren;
    ALIVE_KIDS = numChildren;
    // PRE-FORKING THE SERVER TO GIVEN NUYMBER OF CHILDREN
    for (i = 0; i < numChildren; i++){
        CHECKNE(getpid());
        if (getpid() == FATHER){
            CHECKNE(childpid = fork());
            if (childpid > 0){
                CPID[i] = childpid;
                DEBUG("%s (Create processes) i : % d process ID : % d parent ID :% d child ID :% d", SERVER, i, getpid(), getppid(), childpid);
            }
                
        }
    }
    if (getpid() == FATHER){
        struct sockaddr_in server_addr, client_addr;

        // Init address struct
        bzero(&server_addr, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        server_addr.sin_port = htons(portNumber);

        // CREATING AND INITIALIZING THE SERVER (CREATE SOCKET, BIND, LISTEN)>>>>>
        DEBUG("%s-(%s) Create Server Socket...", SERVER, "Parent");
        CHECKNE(mstsockfd = socket(AF_INET, SOCK_STREAM, 0));
        DEBUG("%s-(%s) Address reuse...", SERVER, "Parent");
        CHECKNE(setsockopt(mstsockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(reuse)));
        DEBUG("%s-(%s) Bind Server socket...", SERVER, "Parent");
        CHECKNE(bind(mstsockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)));
        DEBUG("%s-(%s) Listen socket...", SERVER, "Parent");
        CHECKNE(listen(mstsockfd, SERVER_BACKLOG));
        char ip[30]; strcpy(ip, (char*)inet_ntoa((struct in_addr)server_addr.sin_addr));
        DEBUG("%s-(%s) Socket created successfully! In ip [%s], port [%d], socket descriptor [%d]", SERVER, "Parent", ip, portNumber, mstsockfd);


        fd_set activefds, readfds;
        //  INITIALIZE THE SET OF ACTIVE SOCKETS
        //clear the socket set
        FD_ZERO(&activefds);
        //add master socket to set
        FD_SET(mstsockfd, &activefds);
        while (TRUE){
            runParent(activefds, readfds, pip, mstsockfd, client_addr);
        }
    }
    else if (childpid == 0){
        runChild(pip);
        DEBUG(" child got killed.");
    }
}


void runChild(int pip[2]){

    commandPackage reccp;
    handlePackage rechp;
    //term getting value from commandExecution
    // term == -1   timeToStop
    // term == 0    end
    // term == 1    the command executed
    int term, ret;
    while (TRUE){
        ret=read(pip[READ], &rechp, sizeof(handlePackage));
        reccp = rechp.cp;

        if (strcmp(reccp.command, "EOF") == 0){
            sleep(10);
            int j;
            for(j=0;j<10;j++){
                response(rechp,"EOC");
                sleep(1);
            }
            continue;
        }
        term = commandExecution(rechp);
        char com[BUFSIZE];
        strcpy(com, reccp.command);
        char ip[30];
        strcpy(ip, (char*)inet_ntoa((struct in_addr)rechp.client_addr.sin_addr));
        DEBUG("%s-(Child)    \033[32;1m[Received]\033[37;1m Line: [%d], Send to Port: [%d], Addr: [%s], Command: [%s]", SERVER, reccp.lineNumber, reccp.port, ip, com);
        if (term == 0){  
            kill(getppid(),SIGEND);
            kill(getpid(),SIGINT);
            exit(EXIT_SUCCESS);
        }
        else if (term == -1){
            kill(getppid(), SIGTTS);
        }              
    }
}

/**
    @brief: 
    Calling only by children. Take a command from the pipe client, gives it to parser and execute the returned command via popen(). 

    @param: 
    The command about to execute via popen()
    
    @return:
    return == -1   timeToStop
    return == 0    end
    return == 1    the command executed
*/
int commandExecution(handlePackage hp)
{
    char command[BUFSIZE];
    //GIve the line to parser and take the command via "command"
    int comRetVal = commToExecute(hp.cp.command, command);
    // DEBUG("COMMAND GOT IS : %s", command);
    FILE *fp, *sp;
    char path[512];
    if (strcmp(command, "end") == 0){
        return 0;
    }
    else if (strcmp(command, "timeToStop") == 0){
        return -1;
    }
    else{
        char execCommand[BUFSIZE] = "";
        
        if (comRetVal==0)
            strcat(execCommand, command);
        /* Open the command for reading. */
        sleep(2);
        CHECKNU(fp = popen(execCommand, "r"));

        int buffSize = TEXT_RESPONSE_LEN;
        char *newBuffer = (char *)malloc(buffSize);
        char buf[15];
        // snprintf(buf, 15 , "CHILD %d \n", getpid());
        strcpy(buf, "");
        strcpy(newBuffer, buf );
        /* Read the output a line at a time - output it. */
        while (fgets(path, sizeof(path), fp) != NULL){
            if (strlen(newBuffer) + strlen(path) < buffSize){
                strcat(newBuffer, path);
            }
            else{
                response(hp,newBuffer);
                newBuffer = (char *)malloc(buffSize);
                strcpy(newBuffer, path);
            }
        }
        
        response(hp,newBuffer);
        sleep(2);
        pclose(fp);
        return 1;
    }
    // return 1;
}


void runParent( fd_set activefds,fd_set readfds , int pip[2], int mstsockfd, struct sockaddr_in client_addr){
    signal(SIGEND, endChild); 
    signal(SIGTTS, timeToStop);
    signal(SIGPIPE, SIG_IGN);   

    int activity, addr_size, client_socket;
    DEBUG("%s-(%s) Waiting client to connect...", SERVER, "Parent");

    readfds = activefds;
    //wait for an activity on one of the sockets , timeout is NULL 
    activity = select(FD_SETSIZE, &readfds, NULL, NULL, NULL);
    CHECKNE(activity);
    // if (activity < 0){
    //     perror("select");
    //     exit(EXIT_FAILURE);
    // }
    for (int i = 0; i < FD_SETSIZE; i++){
        if (FD_ISSET(i, &readfds)){
            if (i==mstsockfd){
                addr_size = sizeof(struct sockaddr_in);
                CHECKNO(client_socket = accept(mstsockfd, (struct sockaddr *)&client_addr, (socklen_t *)&addr_size));
                DEBUG("%s-(%s) Client connected successfully!", SERVER, "Parent");
                FD_SET(client_socket, &activefds);
            }
        }
        else if (FD_ISSET(i, &activefds))
        {
            if (readFromConnection(i, pip, client_addr) <0) {
                sleep(1);
                close(i);
                FD_CLR(i, &activefds);
            }
        }
    }
}
int readFromConnection(int client_socket,int pip[2], struct sockaddr_in client_addr){
    commandPackage lastPackage = {.command="", .address="", .port=0, .lineNumber=0};
    int ret;
    while (TRUE){
        size_t bytes_read;
        commandPackage cp;
        handlePackage hp;
        CHECKNO(bytes_read = recv(client_socket, &cp, sizeof(commandPackage), 0));
        if (strcmp(cp.command, "EOF") == 0){
            memcpy(&lastPackage, &cp, sizeof(commandPackage));
            hp.client_addr = client_addr;
            hp.cp = cp;
            // Write the handlepackage
            ret=write(pip[WRITE], &hp, sizeof(handlePackage));
            sleep(1);
            close(client_socket);
            
            DEBUG("%s-(%s) Closing current connection...", SERVER, "Parent");
            return -1;
        }
        else if (memcmp (&cp, &lastPackage, sizeof(commandPackage)) == 0) {
            DEBUG("%s-(%s) [\033[31;1m WARNING \033[37;1m] Client disconnected for some reason...", SERVER, "Parent");
            return -1;
        }
        memcpy(&lastPackage, &cp, sizeof(commandPackage));

        hp.client_addr = client_addr;
        hp.cp = cp;
        // Write the handlepackage
        ret=write(pip[WRITE], &hp, sizeof(handlePackage));    
    }
}
void menu(){
    printf("Usage: ./remoteServer portNumber numChildren\n");
}

/* Wait for all dead child p r o c e s s e s */
void sigchld_handler(int sig){
    while (waitpid(-1, NULL, WNOHANG) > 0){
        ;
    }
}
