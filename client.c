#include "util.h"


int sockfd;

void signal_handler(int);

int main(int argc, char *argv[])
{
    int  portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
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

    t_bool online = true;

    signal(SIGINT, signal_handler);


    while(online){
        printf("Please enter the message: ");
        bzero(buffer,BUFFER_SIZE);
        fgets(buffer,BUFFER_SIZE-1,stdin);
        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0) 
             error("ERROR writing to socket");
        if(strcmp(buffer, quit)==0) online = false;
        if(n==0){
            error("Server is down");
            online = false;
         }
        printf("My message: %s",buffer);

        bzero(buffer,BUFFER_SIZE);
        n = read(sockfd,buffer,BUFFER_SIZE-1);
        if (n < 0) 
             error("ERROR reading from socket");

         if(n==0){
            error("Server is down");
            online = false;
         }
        printf("%s\n",buffer);

    }

    close(sockfd);
    return 0;
}

void signal_handler(int signum){
    printf("Caught signal %d\n", signum);

    char buffer[256];
    strcpy(buffer, quit);
    int n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    if(n==0){
        error("Server is down");
     }
    printf("Exiting application");

    bzero(buffer,BUFFER_SIZE);
    n = read(sockfd,buffer,BUFFER_SIZE-1);
    if (n < 0) 
         error("ERROR reading from socket");
    close(sockfd);
    exit(signum);

}