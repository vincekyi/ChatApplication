/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include "util.h"

int num_users = 0; // keeps track of users

struct User users[MAX_USERS];

int sockfd;
int child_sockfd;
int new_process = 0;
void main_signal_handler(int signum){
    printf("Caught signal %d\n", signum);
    for(int i = 0; i < num_users; i++){
        close(users[i].sockfd);
        printf("Closed FD %d from process id %d\n", users[i].sockfd, users[i].pid);
    }
    close(sockfd);
    exit(signum);

}

void child_signal_handler(int signum){
    printf("PID %d caught signal %d\n", new_process, signum);
    close(child_sockfd);
    exit(signum);

}


void communicate(int sockfd, int user_id){
    char buffer[BUFFER_SIZE];
    bzero(buffer,BUFFER_SIZE);
     int n = read(sockfd,buffer,BUFFER_SIZE-1);
     if (n < 0) error("2ERROR reading from socket");
     buffer[strlen(buffer)-1] = 0;
     printf("User %d's username is %s\n", user_id, buffer);
     char newUserMessage[BUFFER_SIZE];
     int messageSuccess = sprintf (newUserMessage, "Your username is %s and your id is %d", buffer, user_id);
     n = write(sockfd,newUserMessage,BUFFER_SIZE);
     printf("sent username confirmation\n");
     if (n < 0) error("ERROR writing to socket");


    t_bool online = true;
    if (sockfd < 0) 
          error("ERROR on accept");
     while(online){
         bzero(buffer,BUFFER_SIZE);
         int n = read(sockfd,buffer,BUFFER_SIZE-1);
         if (n < 0) error("1ERROR reading from socket");
         printf("User %d: %s", user_id, buffer);
         n = write(sockfd,"Server got your message",23);
         printf("server sent message");
         if (n < 0) error("ERROR writing to socket");
         if(strcmp(buffer, quit)==0){
            printf("%d has quit\n", user_id);
            online = false;
         } 
    }
    exit(0);
}

int main(int argc, char *argv[])
{
     int newsockfd, portno;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }

     // AF_INET: Internet domain
     //SOCK_STREAM: continuous stream (Uses TCP) compared to SOCK_DGRAM: chunks (Uses UDP)
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET; //address family
     serv_addr.sin_addr.s_addr = INADDR_ANY; //gets address of host
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5); // 5 is number of connections that can be waiting while process is handling a 
     clilen = sizeof(cli_addr);

     //reap children
     signal(SIGCHLD, SIG_IGN);
     int pid;
     while (1)
    {


        newsockfd = accept(sockfd,
                   (struct sockaddr *) &cli_addr, &clilen); // accept blocks until client connects
        if (newsockfd < 0)
            error("ERROR on accept");

        new_process++;
        pid = fork();
        if (pid < 0)
            error("ERROR on fork");
        if (pid == 0)
        {
            child_sockfd = newsockfd;
            close(sockfd);
            signal(SIGINT, child_signal_handler);
            communicate(newsockfd, num_users);
        }
        else{
            close(newsockfd);
            users[num_users].sockfd = newsockfd;
            users[num_users].online = true;
            users[num_users].pid = new_process;
            num_users++;

            signal(SIGINT, main_signal_handler);
            
        }
    } /* end of while */

     
     close(sockfd);
     return 0; 
}