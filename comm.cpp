#include "comm.h"

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void server(int portno, double * ppv, int userNum, int machineNum){
    int sockfd, newsockfd;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    /* if (argc < 2) { */
    /*     fprintf(stderr,"ERROR, no port provided\n"); */
    /*     exit(1); */
    /* } */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    /* portno = atoi(argv[1]); */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
                sizeof(serv_addr)) < 0) 
        error("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, 
            (struct sockaddr *) &cli_addr, 
            &clilen);
    if (newsockfd < 0) 
        error("ERROR on accept");

    int node;
    double value;
    int finishedMachCnt = 0;
    while(finishedMachCnt < machineNum){
        n = read(newsockfd, &node, sizeof node);
        if (n < 0)
            error("ERROR reading from socket");
        if(node == -1){
            finishedMachCnt++;
            continue;
        }
        n = read(newsockfd, &value, sizeof value);
        if (n < 0)
            error("ERROR reading from socket");
        /* printf("Here is the message: %d %lf\n", node, value); */
        if(node >= userNum){
            error("ERROR node ID invalid");
        }
        else{
            ppv[node] += value;
        }
    }
    close(newsockfd);
    close(sockfd);
}

void client(int portno, char* hostName, double * ppv, int userNum){
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    /* if (argc < 3) { */
    /*    fprintf(stderr,"usage %s hostname port\n", argv[0]); */
    /*    exit(0); */
    /* } */
    /* portno = atoi(argv[2]); */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(hostName);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    /* fgets(buffer,255,stdin); */

    for(int i = 0; i < userNum; i++){
        if(ppv[i] < 1e-8)
            continue;
        n = write(sockfd,&i,sizeof i);
        if (n < 0) 
            error("ERROR writing to socket");
        n = write(sockfd,&ppv[i],sizeof ppv[i]);
        if (n < 0) 
            error("ERROR writing to socket");
    }
    /* double buffer[] = {1.2, 2.2, 3.2, 4.2}; */
    /* for(int i = 0; i < 4; i++){ */
    /*     n = write(sockfd,&i,sizeof i); */
    /*     if (n < 0) */ 
    /*         error("ERROR writing to socket"); */

    /*     n = write(sockfd,&buffer[i],sizeof buffer[i]); */
    /*     if (n < 0) */ 
    /*         error("ERROR writing to socket"); */
    /* } */
    int term = -1;
    n = write(sockfd,&term,sizeof term);
    if (n < 0) 
        error("ERROR writing to socket");
    close(sockfd);
}

