#include "handling.h"

#define SERVER_BACKLOG 5
double LAST_REQUEST;

void menu();
void sigchld_handler(int sig);
void runServer(int numChildren, int portNumber);
void runChild(int pip[2]);
void runParent(int pip[2], int mstsockfd, struct sockaddr_in client_addr);
int commandExecution(handlePackage hp);

// Children PIDs, Status and number of alive kids
volatile int CPID[1024];
volatile int KIDS_NUM;
volatile int ALIVE_KIDS;
volatile bool EXIT=FALSE;

void endChild(){
    signal(SIGEND, endChild); 
    int pid;    //todo 

    ALIVE_KIDS--;
    CPID[pid]=-1;
}

void timeToStop(){
    signal(SIGTTS, timeToStop);
    int i;
    char mess[200];    
    for(i=0;i<KIDS_NUM; i++){
        if(CPID[i]>0){
            if (kill(CPID[i], SIGKILL) == 0){
                if(DEBUG_S==FALSE){
                    snprintf(mess, sizeof(mess), "%s-(Child)    \033[31;1m[KILL]\033[37;1m Process with PID: [%d] suicide.", SERVER, getpid());
                    PRINTMESS(COLOR("KILL",YELLOW), mess);
                }
                else{
                    DEBUG("%s-(Child:%d)    \033[31;1m[KILL]\033[37;1m Process with PID: [%d] suicide.", SERVER, ALIVE_KIDS, getpid());
                }
            }
            // else{
            //     snprintf(mess, sizeof(mess), "%s-(Child)    \033[31;1m[END]\033[37;1m Child with PID: [%d] suicide.", SERVER, CPID[i]);
            //     PRINTMESS(COLOR("ERROR",RED), mess);
            // }
        }
    }
    ALIVE_KIDS=0;
}

bool alive(){
    int i;
    int alive=FALSE;
    for(i=0;i<KIDS_NUM; i++){
        if(kill(CPID[i], 0) == 0){
            return alive = TRUE;
        }
    }
    return alive;
}

void killFather(){
    if ((ALIVE_KIDS <= 0 || alive()==FALSE)){
        char mess[200];
        snprintf(mess, sizeof(mess), "%s-(Father)    \033[31;1m[KILL]\033[37;1m Process with PID: [%d] suicide.", SERVER, getpid());
        if (EXIT == FALSE){
            PRINTMESS(COLOR("KILL",YELLOW), mess);
            EXIT = TRUE;
        }
        
        exit(EXIT_SUCCESS);
    }
}

#define ALARM_TIME 3
void alarm_handler(){
    signal(SIGALRM, alarm_handler);
    killFather();
    alarm(ALARM_TIME);
}

//-------------------------------------------------------------------------
int main(int argc, char *argv[]){
    signal(SIGPIPE, SIG_IGN);
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
        while (TRUE){
            runParent(pip, mstsockfd, client_addr);
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
            char mess[200];
            if(DEBUG_S==FALSE){
                snprintf(mess, sizeof(mess), "%s-(Child)    \033[31;1m[KILL]\033[37;1m Process with PID: [%d] suicide.", SERVER, getpid());
                PRINTMESS(COLOR("KILL",YELLOW), mess);
            }
            else{
                DEBUG("%s-(Child:%d)    \033[31;1m[KILL]\033[37;1m Process with PID: [%d] suicide.", SERVER, ALIVE_KIDS, getpid());
            }
            
            kill(getppid(), SIGEND);
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
        pclose(fp);
        // DEBUG("%s-(%s) ... FINISHED COMMAND EXECUTION ...", SERVER, "Child");
        return 1;
    }
    // return 1;
}


void runParent(int pip[2], int mstsockfd, struct sockaddr_in client_addr){
    signal(SIGALRM, alarm_handler);
    signal(SIGEND, endChild); 
    signal(SIGTTS, timeToStop);
    
    signal(SIGPIPE, SIG_IGN);   

    fd_set readfds;
    int activity, addr_size, client_socket;

    DEBUG("%s-(%s) Waiting client to connect...", SERVER, "Parent");

    killFather();

    //clear the socket set
    FD_ZERO(&readfds);
    //add master socket to set
    FD_SET(mstsockfd, &readfds);
    
    //wait for an activity on one of the sockets , timeout is NULL ,
    //so wait indefinitely
    // TODO    check if it needs timeout
    activity = select(mstsockfd + 1, &readfds, NULL, NULL, NULL);
    
    if ((activity < 0) && (errno != EINTR)){
        PRINTMESS(COLOR("ERROR", RED), "Select error");
    }
    if (FD_ISSET(mstsockfd, &readfds)){
        addr_size = sizeof(struct sockaddr_in);
        //TODO chgange to handlepackage struct
        CHECKNO(client_socket = accept(mstsockfd, (struct sockaddr *)&client_addr, (socklen_t *)&addr_size));
        DEBUG("%s-(%s) Client connected successfully!", SERVER, "Parent");

        // Last package init
        commandPackage lastPackage = {.command="", .address="", .port=0, .lineNumber=0};
        int ret;
        alarm(ALARM_TIME);
        while (TRUE){
            
            killFather();  

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
                close(client_socket);
                
                DEBUG("%s-(%s) Closing current connection...", SERVER, "Parent");
                break;
            }
            else if (memcmp (&cp, &lastPackage, sizeof(commandPackage)) == 0) {
                DEBUG("%s-(%s) [\033[31;1m WARNING \033[37;1m] Client disconnected for some reason...", SERVER, "Parent");
                break;
            }
            memcpy(&lastPackage, &cp, sizeof(commandPackage));

            hp.client_addr = client_addr;
            hp.cp = cp;
            // Write the handlepackage
            ret=write(pip[WRITE], &hp, sizeof(handlePackage));
                
        }
    }
}

void menu(){
    printf("Usage: ./remoteServer portNumber numChildren\n");
}

/* Wait for all dead child p r o c e s s e s */
void sigchld_handler(int sig){
    while (waitpid(-1, NULL, WNOHANG) > 0){
        killFather();
    }
}
