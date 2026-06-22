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

#include <vector>
#include <fstream>
#include <iostream>

#define CHUNK 4096
#define PORT "3490"

void* fet_in_addr(struct sockaddr* sa){
    if(sa->sa_family==AF_INET)
        return &((struct sockaddr_in*)sa)->sin_addr;
    return &((struct sockaddr_in6*)sa)->sin6_addr;
}

//main client body
int main(int argc, char *argv[]){
    int fd;
    struct addrinfo hints, *res, *p;
    int status;
    std::vector<char> red(CHUNK);
    memset(&hints, 0, sizeof hints);
    hints.ai_family=AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if(argc!=2){
        perror("Domain not found, make sure to type domain..");
        exit(1);
    }
    if((status=getaddrinfo(argv[1],PORT,&hints,&res))!=0){
        perror("getaddrinfo");
        exit(1);
    }
    for(p=res;p!=NULL;p=p->ai_next){
        if((fd=socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1){
            perror("socket");
            continue;
        }
        if(connect(fd,p->ai_addr,p->ai_addrlen)==-1){
            perror("connect");
            continue;
        }
        break;
    }
    freeaddrinfo(res);
    if(p==NULL){
        perror("sock/connect");
        exit(1);
    }
    printf("connected to domain:%s\n",argv[1]);
    //get the file extention name
    uint32_t ext_len;
    int r;
    if(recv(fd,&ext_len,sizeof ext_len,MSG_WAITALL)<=0){
        perror("recv extention size");
        exit(1);
    }
    ext_len = ntohl(ext_len);
    std::string s(ext_len,'\0');
    if(recv(fd,&s[0],ext_len,MSG_WAITALL)<=0){
        perror("recv extention");
        exit(1);
    }
    std::fstream file("op"+s,std::ios::out|std::ios::binary);
    if(!file.is_open()){
        // fprintf(stderr,"file op%s failed to open\n",s); // you gotta use s.c_string() cuz fprintf is ancient
        std::cerr<<"file op"<<s<<" failed to open"<<std::endl;
        exit(1);
    }
    while(1){
        int r;
        if((r=recv(fd,red.data(),CHUNK,0))==-1){
            perror("recv");
            break;
        }
        if(r==0)break;
        file.write(red.data(),r);
        printf("got some data %d bytes\n",r);
    }
    close(fd);
    printf("probably got the file, SHA-256 what?\n");
    return 0;
}