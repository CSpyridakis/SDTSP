#include <netinet/in.h> // Internet addresses are defined here
#include <arpa/inet.h>
#include <unistd.h> // Fork
#include "handling.h"

#include <signal.h> /* signal */

#define SERVER_BACKLOG 5
double LAST_REQUEST;

void menu();
void sigchld_handler(int sig);
void runServer(int numChildren, int portNumber);
void runChild(int pip[2]);
void runParent(int pip[2], int mstsockfd, struct sockaddr_in client_addr);
int commandExecution(char *givenCommand);

//-------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    signal(SIGPIPE, SIG_IGN);
    // Check input parameters
    if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0))
    {
        menu();
        exit(EXIT_SUCCESS);
    }
    else if (argc != 3)
    {
        fprintf(stderr, "Wrong number of arguments!\nPlease run %s -h to see properly usage\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Input parameters to variables
    int portNumber = atoi(argv[1]);
    int numChildren = atoi(argv[2]);
    DEBUG("%s-(Input Parameters) portNumber: %d, numChildren: %d", SERVER, portNumber, numChildren);
    if (portNumber < 1)
    {
        fprintf(stderr, "Port parameters must be acceptable!\nPlease run %s -h to see properly usage\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    else if (numChildren < 1 || numChildren > 1024)
    { // TODO FIX IT
        fprintf(stderr, "Number of childrens must be acceptable\nPlease run %s -h to see properly usage\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Reap dead children a s y n c h r o n o u s l y */
    signal(SIGCHLD, sigchld_handler);
    runServer(numChildren, portNumber);
    sleep(10);
    exit(EXIT_SUCCESS);
}

// TODO in CTR + C signal in client send end to Server

void runServer(int numChildren, int portNumber)
{
    int mstsockfd, pip[2], i;
    int FATHER = getpid();
    pid_t childpid;
    struct sockaddr_in server_addr, client_addr;
    struct hostent *rem;
    char *message = (char *)malloc(sizeof(char) * BUFSIZE);

    // Init address struct
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(portNumber);

    // CREATING AND INITIALIZING THE SERVER (CREATE SOCKET, BIND, LISTEN)>>>>>
    DEBUG("%s-(%s) Create Server Socket...", SERVER, "Parent");
    CHECKNE(mstsockfd = socket(AF_INET, SOCK_STREAM, 0));
    int reuse = 1;
    DEBUG("%s-(%s) Address reuse...", SERVER, "Parent");
    CHECKNE(setsockopt(mstsockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(reuse)));
    DEBUG("%s-(%s) Bind Server socket...", SERVER, "Parent");
    CHECKNE(bind(mstsockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)));
    DEBUG("%s-(%s) Listen socket...", SERVER, "Parent");
    CHECKNE(listen(mstsockfd, SERVER_BACKLOG));

    char ip[30];
    strcpy(ip, (char*)inet_ntoa((struct in_addr)server_addr.sin_addr));
    DEBUG("%s-(%s) Socket created successfully! In ip [%s], port [%d], socket descriptor [%d]", SERVER, "Parent", ip, portNumber, mstsockfd);
    
    DEBUG("%s FATHER's PID %d...", SERVER, FATHER);

    // PIPE creation
    CHECKNO(pipe(pip));

    // PRE-FORKING THE SERVER TO GIVEN NUYMBER OF CHILDREN
    for (i = 0; i < numChildren; i++)
    {
        CHECKNE(getpid());
        if (getpid() == FATHER)
        {
            childpid = fork();
            CHECKNE(childpid);
            if (childpid > 0)
                DEBUG("%s (Create processes) i : % d process ID : % d parent ID :% d child ID :% d", SERVER, i, getpid(), getppid(), childpid);
        }
    }
    if (getpid() == FATHER)
    {
        while (TRUE)
        {
            runParent(pip, mstsockfd, client_addr);
        }
    }
    else if (childpid == 0)
    {
        runChild(pip);
        DEBUG(" child got killed.");
    }
    else
    {
        DEBUG("<<<<<<<<<<<<<<<<< NEITHER FATHER NOR CHILD >>>>>>>>>>>>>>>>>>")
    }
}

void runChild(int pip[2])
{
    commandPackage reccp;
    handlePackage rechp;
    //term getting value from commandExecution
    // term == -1   timeToStop
    // term == 0    end
    // term == 1    the command executed
    int term, ret;
    while (TRUE)
    {
        ret=read(pip[READ], &rechp, sizeof(handlePackage));
        reccp = rechp.cp;
        term = commandExecution(reccp.command);
        char com[BUFSIZE];
        strcpy(com, reccp.command);
        char ip[30];
        strcpy(ip, (char*)inet_ntoa((struct in_addr)rechp.client_addr.sin_addr));
        DEBUG("%s-(Child)    \033[32;1m[Received]\033[37;1m Line: [%d], Send to Port: [%d], Addr: [%s], Command: [%s]", SERVER, reccp.lineNumber, reccp.port, ip, com);
        if (term == 0)
        {
            DEBUG(">>>>>>>>>>>>>>>>>>>>>>>>    %s", reccp.command);
            // break;
        }
        else if (term == -1)
        {
            DEBUG(">>>>>>>>>>>>>>>>>>>>>>>>    %s", reccp.command);
        }     
        else
        {
            response(rechp,rechp.cp.command);
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
int commandExecution(char *givenCommand)
{

    char command[BUFSIZE];
    //GIve the line to parser and take the command via "command"
    int comRetVal = commToExecute(givenCommand, command);
    // DEBUG("COMMAND GOT IS : %s", command);
    FILE *fp, *sp;
    char path[512];
    if (strcmp(command, "end") == 0)
    {
        // DEBUG("END-%s", command);
        return 0;
    }
    else if (strcmp(command, "timeToStop") == 0)
    {
        // DEBUG("END-%s", command);
        return -1;
    }
    else
    {
        return 1;

        // char execCommand[BUFSIZE] = "echo | ";
        // if (comRetVal==0)
        //     strcat(execCommand, command);
        // /* Open the command for reading. */
        // fp = popen(execCommand, "r");
        // if (fp == NULL)
        // {
        //     printf("Failed to run command\n");
        //     exit(1);
        // }
        // int buffSize = 512;
        // char *newBuffer = (char *)malloc(buffSize);
        // char buf[15];
        // snprintf(buf, 15 , "CHILD %d \n", getpid());
        // strcpy(newBuffer, buf );
        // /* Read the output a line at a time - output it. */
        // while (fgets(path, sizeof(path), fp) != NULL)
        // {
        //     if (strlen(newBuffer) + strlen(path) < buffSize)
        //     {
        //         strcat(newBuffer, path);
        //     }
        //     else
        //     {
        //         char *sendinBuf = (char *)malloc(4096);
        //         strcpy(sendinBuf, "echo \" ");
        //         strcat(sendinBuf, newBuffer);
        //         char buf2[30];
        //         char * token = strtok(command, " ");
        //         snprintf(buf2, 30 , "\" >> ./results/%s.txt ", token);
        //         strcat(sendinBuf, buf2);
        //         sp = popen(sendinBuf, "r");
        //         /* close */
        //         pclose(sp);
        //         newBuffer = (char *)malloc(buffSize);
        //     }
        // }
        // pclose(fp);
        // char *sendinBuf = (char *)malloc(4096);
        // strcpy(sendinBuf, "echo \" ");
        // strcat(sendinBuf, newBuffer);
        // char buf2[30];
        // char * token = strtok(command, " ");
        // snprintf(buf2, 30 , "\" >> ./results/%s.txt ", token);
        // strcat(sendinBuf, buf2);
        // fp = popen(sendinBuf, "r");
        // /* close */
        // pclose(fp);
        // DEBUG("FINISHED....................");
    }
    // return 1;
}

void runParent(int pip[2], int mstsockfd, struct sockaddr_in client_addr)
{

    fd_set readfds;
    int activity, addr_size, client_socket;

    DEBUG("%s-(%s) Waiting client to connect...", SERVER, "Parent");
    //clear the socket set
    FD_ZERO(&readfds);
    //add master socket to set
    FD_SET(mstsockfd, &readfds);

    //wait for an activity on one of the sockets , timeout is NULL ,
    //so wait indefinitely
    // TODO    check if it needs timeout
    activity = select(mstsockfd + 1, &readfds, NULL, NULL, NULL);

    if ((activity < 0) && (errno != EINTR))
    {
        printf("select error");
    }
    if (FD_ISSET(mstsockfd, &readfds))
    {
        addr_size = sizeof(struct sockaddr_in);
        //TODO chgange to handlepackage struct
        CHECKNO(client_socket = accept(mstsockfd, (struct sockaddr *)&client_addr, (socklen_t *)&addr_size));
        DEBUG("%s-(%s) Client connected successfully!", SERVER, "Parent");

        int ret;
        while (TRUE)
        {
            size_t bytes_read;
            commandPackage cp;
            handlePackage hp;
            
            CHECKNO(bytes_read = recv(client_socket, &cp, sizeof(commandPackage), 0));
            if (strcmp(cp.command, "EOF") == 0)
            {
                close(client_socket);
                DEBUG("End_Of_FIle");
                DEBUG("%s-(%s) Closing current connection...", SERVER, "Parent");
                break;
            }
            hp.client_addr = client_addr;
            hp.cp = cp;
            // Write the handlepackage
            ret=write(pip[WRITE], &hp, sizeof(handlePackage));
        }
    }
}

void menu()
{
    printf("Usage: ./remoteServer portNumber numChildren\n");
}

/* Wait for all dead child p r o c e s s e s */
void sigchld_handler(int sig)
{
    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;
}
