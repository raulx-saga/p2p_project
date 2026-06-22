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

// #Simple server program
#define PORT "3490"
#define BACKLOG 10

void * get_in_addr(struct sockaddr* addr){
    if(addr->sa_family==AF_INET){
        return &((struct sockaddr_in*)addr)->sin_addr;
    }
    return &((struct sockaddr_in6*)addr)->sin6_addr;
} 

int main(){
    struct addrinfo hints, *res, *p;
    int status;
    int fd, new_fd;
    int yes =1;
    char str[INET6_ADDRSTRLEN];
    struct sockaddr_storage their_addr;
    socklen_t size_sin;
    char buf[100]="Hola Amigo, Internet Edition\n";
    int buf_size =100;

    memset(&hints,0,sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if((status = getaddrinfo(NULL,PORT,&hints,&res))!=0){
        perror("server: getaddrinfo()");
        exit(1);
    }
    for(p=res;p!=nullptr;p=p->ai_next){
        if((fd=socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1){
            perror("servor: socket()");
            continue;
        }
        if(setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes)==-1){
            perror("server:setsockopt");
            exit(1);
        }
        if(bind(fd,p->ai_addr,p->ai_addrlen)!=0){
            perror("server: bind()");
            close(fd);
            continue;
        }
        break;
    }
    freeaddrinfo(res);
    if(p==nullptr){
        perror("list samapt");
        exit(1);
    }
    if(listen(fd,BACKLOG)==-1){
        perror("listen");
        exit(1);
    }
    printf("server waiting for connections....\n");
    while(1){
        size_sin = sizeof(their_addr);
        new_fd = accept(fd,(struct sockaddr*)&their_addr,&size_sin);
        if(new_fd==-1){
            perror("server: accept");
            continue;
        }
        //print the connection we got. 
        inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr*)&their_addr),str,sizeof str);
        printf("got connection from %s\n",str);
        //send the data and fuck off
        int x = send(new_fd,buf,buf_size,0);
        if(x==-1){
            perror("send");
            close(new_fd);
            continue;
        }
        printf("data sent succesfully!\n");
        close(new_fd);
    }
    return 0;
}