#include "handling.h"

volatile double LAST_RECEIVED;
#define ALARM_TIME 100

void alarm_handler(){
    signal(SIGALRM, alarm_handler);
    if(gettime()-LAST_RECEIVED>=TIMEOUT){
        DEBUG("%s-(%s) [TIMEOUT] Close receiving process", CLIENT, "Child");
        exit(EXIT_SUCCESS);
    }
    alarm(ALARM_TIME);
}

void menu(){
    printf("Usage: ./remoteClient serverName serverPort receivePort inputFileWithCommands\n");
}

/**
    @brief:

    @param:

    @return:
*/
int receiveFromServer(char *process, int receivePort){

    signal(SIGALRM, alarm_handler);
    int sockfd, reuse = 1, ret;

    unsigned int serv_len;

     // Init address struct
    struct sockaddr_in receive_addr;
    bzero(&receive_addr,sizeof(receive_addr));
    memset(&receive_addr, 0, sizeof(receive_addr)); 
    receive_addr.sin_family = AF_INET;
    receive_addr.sin_addr.s_addr = INADDR_ANY;
    receive_addr.sin_port = htons(receivePort);

    char ip[30];
    strcpy(ip, (char*)inet_ntoa((struct in_addr)receive_addr.sin_addr));

    DEBUG("%s-(%s) Create Client Socket...", CLIENT, process);
    CHECKNE(sockfd=socket(AF_INET, SOCK_DGRAM, 0));
    DEBUG("%s-(%s) Address reuse...", CLIENT, process);
    CHECKNE(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*) &reuse, sizeof(reuse)));
    // struct timeval tv;
    // tv.tv_sec = 15;
    // CHECKNE(setsockopt(sockfd,SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)));
    DEBUG("%s-(%s) Bind client's socket...", CLIENT, process);
    CHECKNE(ret = bind(sockfd, (struct sockaddr *)&receive_addr, sizeof(struct sockaddr)));
    DEBUG("%s-(%s) UDP listener created in ip [%s], port [%d], socket descriptor [%d]", CLIENT, process, ip, receivePort, sockfd);
    
    // init address struct
    struct sockaddr_in server;
    double startConn=gettime();
    LAST_RECEIVED=gettime();
    alarm(ALARM_TIME);
    while(TRUE){    
        serv_len = sizeof(server);
        responsePackage rp;

        ret = recvfrom(sockfd, &rp, sizeof(rp), 0, (struct sockaddr *)&server, &serv_len);

        if( ret < 0){
            DEBUG("%s-(%s)    \033[31;1m[ERROR]\033[37;1m In receiving package", CLIENT, process);
            continue;
        }   

        LAST_RECEIVED=gettime();
        
        char res[BUFSIZE];
        strcpy(res, rp.response);
        DEBUG("%s-(%s)    \033[35;1m[Received]\033[37;1m Line: [%d] Response: [%s]", CLIENT, process, rp.lineNumber, res);
        
        if(strcmp(res, "EOC") == 0){break;}

        writeToFile(process, receivePort, rp.lineNumber, res);
    }
    DEBUG("%s-(%s) Close receiving process", CLIENT, process);
    sleep(1);
    close(sockfd);
}

/**
    @brief:

    @param:

    @return:
*/
int sentToServer(char *process, char *serverName, int serverPort, int receivePort, char *inputFileWithCommands){

    char buffer[BUFSIZE];

    DEBUG("%s-(%s) Get hostname...", CLIENT, process);
    struct hostent *remote_addr;
    if(!(remote_addr=gethostbyname(serverName))){FATAL("ERROR with hostname!")}

    DEBUG("%s-(%s) Open input file...", CLIENT, process);
    FILE *fp;
    char *line = NULL ; size_t len = 0 ; ssize_t read;
    CHECKNU(fp = fopen(inputFileWithCommands, "r"));

    DEBUG("%s-(%s) Create Client Socket...", CLIENT, process);
    int sockfd;
    CHECKNO(sockfd=socket(AF_INET, SOCK_STREAM, 0));

    DEBUG("%s-(%s) Convert hostname to address form...", CLIENT, process);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(serverPort);
    serv_addr.sin_addr.s_addr = *(long *)(remote_addr->h_addr_list[0]);

    DEBUG("%s-(%s) Client try to connect to server...", CLIENT, process);
    CHECKNO(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)));

    DEBUG("%s-(%s) Start sending packets...", CLIENT, process);
    int cntMess = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        //Create the command package
        commandPackage cp;
        if (line[strlen(line)-1]=='\n') line[strlen(line)-1]='\0';
        strcpy(cp.command, line);
        strcpy(cp.address, ""); //TODO
        cp.port=receivePort;
        cp.lineNumber=cntMess+1;

        if (strlen(line) + 1 >= BUFSIZE){
            PRINTMESS(COLOR("ERROR", YELLOW), "Line is bigger than BUFSIZE"); 
            cntMess++ ; 
            if(cntMess%10==0) {sleep(SLEEP);};
            continue;
        }

        char add[BUFSIZE]; strcpy(add, cp.address); char com[BUFSIZE]; strcpy(com, cp.command);
        DEBUG("%s-(%s)    \033[34;1m[Packet data]\033[37;1m Line: [%d], Port: [%d], Addr: [%s], Command: [%s]", CLIENT, process, cp.lineNumber, cp.port, add, com);
        CHECKNE(send(sockfd, &cp, sizeof(cp), 0));

        cntMess++;
        if(cntMess%10==0) {sleep(SLEEP);};
    }
    commandPackage cp;
    strcpy(cp.command, "EOF");
    strcpy(cp.address, ""); //TODO
    cp.port=receivePort;
    cp.lineNumber=cntMess+1;
    CHECKNE(send(sockfd, &cp, sizeof(cp), 0));

    //TODO
    // CHECKNE(shutdown(sockfd, SHUT_RDWR));

    DEBUG("%s-(%s) Client has sent all requests!", CLIENT, process);
    // Free resources and exit
    sleep(1); 
    close(sockfd);
    fclose(fp); 
    free(line);
}


//-------------------------------------------------------------------------
int main(int argc, char *argv[]){
    signal(SIGPIPE, SIG_IGN);
    // Check input parameters
    if(argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0 )) {menu() ; exit(EXIT_SUCCESS);}
    else if (argc != 5){
        fprintf(stderr, "Wrong number of arguments!\nPlease run %s -h to see properly usage\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Create local variables and copy input parameter to them
    char *serverName = parToVar(argv[1]);
    int serverPort = atoi(argv[2]);
    int receivePort = atoi(argv[3]);
    char *inputFileWithCommands = parToVar(argv[4]);
    DEBUG("%s-(Input Parameters) ServerName: %s, serverPort: %d, receivePort: %d, inputFileWithCommands: %s", CLIENT, serverName, serverPort, receivePort, inputFileWithCommands);
    if (serverPort<1 || receivePort<1){
        fprintf(stderr, "Port parameters must be acceptable!\nPlease run %s -h to see properly usage\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    DEBUG("%s Create process for receiving messages...", CLIENT);
    pid_t childpid;
    CHECKNO(childpid = fork());

    if (childpid == 0){
        receiveFromServer("Child", receivePort);
    }
    else{
        sentToServer("Parent", serverName, serverPort, receivePort, inputFileWithCommands);
    }

    free(serverName) ; free(inputFileWithCommands) ;
    exit(EXIT_SUCCESS);
}
