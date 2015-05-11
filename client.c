#include "util.h"


int sockfd;

void signal_handler(int signum){
    printf("Caught signal %d\n", signum);
    close(sockfd);
    exit(signum);

}

int main(int argc, char *argv[])
{
    int portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[BUFFER_SIZE];
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
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) //connect to server
        error("ERROR connecting");

    struct timeval t;    
    t.tv_sec = 0;
    t.tv_usec = 0;
    setsockopt(
        sockfd,     // Socket descriptor
        SOL_SOCKET, // To manipulate options at the sockets API level
        SO_RCVTIMEO,// Specify the receiving or sending timeouts 
        (char *)&t, // option values
        sizeof(t) 
    );   

    t_bool online = true;

    signal(SIGINT, signal_handler);
    // create username
    printf("Enter your username: ");
    bzero(buffer,BUFFER_SIZE);
    fgets(buffer,BUFFER_SIZE-1,stdin);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");

    bzero(buffer,BUFFER_SIZE);
    n = read(sockfd,buffer,BUFFER_SIZE-1);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("%s\n",buffer);
    printf("Conversation:***\n");



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
         printf("%d, %d", (int)buffer[0], n);
        printf("%s\n",buffer);

    
    close(sockfd);
    return 0;
}