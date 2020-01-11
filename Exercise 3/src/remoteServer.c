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
    DEBUG("%s-(%s) Socket created successfully! Socket descriptor: %d", SERVER, "Parent", mstsockfd);
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
                DEBUG(" i : % d process ID : % d parent ID :% d child ID :% d \n ", i, getpid(), getppid(), childpid);
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
    }
    else{
        DEBUG("<<<<<<<<<<<<<<<<NEITHER FATHER NOR CHILD>>>>>>>>>>>>>>>>>>")
    }
}

void runChild(int pip[2])
{
    commandPackage reccp;
    while (TRUE)
    {
        read(pip[READ], &reccp, sizeof(commandPackage));
        DEBUG("READING --- command %d:    %s", reccp.lineNumber, reccp.command);
        if (strcmp(reccp.command, "end") == 0)
        {
            DEBUG("READING command %s", reccp.command);
            break;
        }
    }
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
        CHECKNO(client_socket = accept(mstsockfd, (struct sockaddr *)&client_addr, (socklen_t *)&addr_size));
        DEBUG("%s-(%s) Client connected successfully!", SERVER, "Parent");

        while (TRUE)
        {
            size_t bytes_read;
            commandPackage cp;
            CHECKNO(bytes_read = recv(client_socket, &cp, sizeof(commandPackage), 0));
            if (strcmp(cp.command, "EOF") == 0)
            {
                close(client_socket);
                DEBUG("End_Of_FIle");
                DEBUG("%s-(%s) Closing current connection...", SERVER, "Parent");
                break;
            }
            write(pip[WRITE], &cp, sizeof(commandPackage));
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
