#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <signal.h>
#include <sys/time.h>

typedef enum {
    false = 0,
    true
} t_bool;


const char quit[] = "quit\n";

const int BUFFER_SIZE = 256;
const int MAX_USERS = 10;

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

struct User{
	char user_id[BUFFER_SIZE];
	int sockfd;
	t_bool online;
	int pid;
};