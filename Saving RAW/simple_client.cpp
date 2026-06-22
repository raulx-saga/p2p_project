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

#define PORT "3490"

//main client body
int main(int argc, char *argv[]){
    int fd;
    int status;
    char str[INET6_ADDRSTRLEN];
    int buf_size=100;
    char buff[100];
    if(argc!=2){
        fprintf(stderr,"Client: Domain not found\n");
        exit(2);
    }
    struct addrinfo hints, *res,*p;
    memset(&hints,0,sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if((status = getaddrinfo(argv[1],PORT,&hints,&res))!=0){
        fprintf(stderr,"Client: getaddrinfo()\n");
        exit(2);
    }
    for(p=res;p!=NULL;p=p->ai_next){
        if((fd=socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1){
            fprintf(stderr,"Client: socket()\n");
            continue;
        }
        if(connect(fd,p->ai_addr,p->ai_addrlen)!=0){
            fprintf(stderr,"client: connect()\n");
            continue;
        }
        break;
    }
    freeaddrinfo(res);
    if(p==nullptr){
        fprintf(stderr,"client: connect()/socket()\n");
        exit(1);
    }
    printf("connected, waiting for a msg...\n");
    recv(fd,&buff,buf_size,0);
    //print the recieved block of data
    printf("%s",buff);
    // send(fd,&buff,buf_size,0);
    close(fd);
    return 0;
}