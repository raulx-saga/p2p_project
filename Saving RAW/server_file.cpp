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

#include <fstream>
#include <vector>

// #Simple server program
#define PORT "3490"
#define BACKLOG 10
#define CHUNK 4096

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
    std::vector<char> red(CHUNK);
    int buf_size =CHUNK;

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
        std::fstream file("video.mp4",std::ios::in|std::ios::binary);
        if(!file.is_open()){
            printf("file failed to open\n");
            exit(1);
        }
        //send the file metadata
        std::string s="video.mp4";
        uint32_t ext_len = s.length();
        ext_len = htonl(ext_len);
        if(send(new_fd,&ext_len,sizeof ext_len,0)==-1){
            perror("send");
            break;
        }
        if(send(new_fd,s.c_str(),s.length(),0)==-1){
            perror("send");
            break;
        }
        //send a file to the client 
        while(!file.eof()){
            file.read(red.data(),CHUNK);
            int x = file.gcount();
            if(send(new_fd,red.data(),x,0)==-1){
                perror("send");
                break;
            }
        }
        if(!file.eof())fprintf(stderr,"error sending to client %s\n",str);
        else printf("file sent to client: %s\n",str);
        close(new_fd);
    }
    return 0;
}