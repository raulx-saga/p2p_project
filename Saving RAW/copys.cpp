#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <poll.h>

#define PORT "3490"
#define BACKLOG 10

int main(){
    struct addrinfo hints, *res, *p;
    int status;
    int fd, newfd;
    int yes =1;
    struct sockaddr_storage theiraddr;
    socklen_t theiraddr;
    struct pollfd fds[10];

    memset(&hints,0,sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    status = getaddrinfo(NULL,PORT,&hints,&res);
    if(status!=0){
        perror("getaddrinfo()");
        exit(1);
    }

    for(p= res;p!=NULL;p=p->ai_next){
        if(fd = socket(p->ai_family,p->ai_socktype,p->ai_protocol)==-1){
            perror("socket()");
            continue;
        }
        if(setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes)==-1){
            perror("setsockopt");
            continue;
        }
        if(bind(fd,p->ai_addr,p->ai_addrlen)){
            perror("bind");
            continue;
        }
        break;
    }
    if(!p){
        fprintf(stderr,"list samapt....\n");
        exit(1);
    }
    freeaddrinfo(res);
    if(listen(fd,BACKLOG)==-1){
        perror("listen");
        exit(1);
    }
    fds[0].fd = fd;
    fds[0].events = POLLIN;
    printf("listening on port %s\n",PORT);
    while(1){
        int ready = poll(fds,10,0);
        if(ready==-1){
            perror("poll");
            continue;
        }
        if(fds[0].events&&fds[0].revents){
            printf("new connection is ready to be accptd\nattempting to add to the list fds..n");
            if(fds[9].fd>10)
        }
        for(int i =1;i<10;i++){
            if(fds[i])
        }
    }

}