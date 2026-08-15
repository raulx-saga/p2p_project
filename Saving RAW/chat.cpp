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

#include <vector>
#include <iostream>

#define PORT "3490"
#define BACKLOG 10
// using namespace std;

const char* inet_ntop2(void *raddr, char* rIP, int size){
    sockaddr_storage * sas = (sockaddr_storage *)raddr;
    sockaddr_in * sa4;
    sockaddr_in6 * sa6;
    void * src;
    switch (sas->ss_family)
    {
    case AF_INET:
        sa4 = (sockaddr_in *)raddr;
        src = &(sa4->sin_addr);
        break;
    case AF_INET6:
        sa6 = (sockaddr_in6 *)raddr;
        src = &(sa6->sin6_addr);
        break;
    default:
        return NULL;
    }
    return inet_ntop(sas->ss_family,src,rIP,size);
}

int get_listner_socket(){
    int status;
    int yes=1;
    int fd;
    addrinfo hints, *res,*p;
    memset(&hints,0,sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // soc ,

    if((status = getaddrinfo(NULL,PORT,&hints,&res))!=0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(-11);
    }
    for(p=res;p!=nullptr;p=p->ai_next){
        fd = socket(p->ai_family,p->ai_socktype,p->ai_protocol);
        if(fd==-1){
            perror("socket");
            continue;
        }
        if(setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes)==-1){
            fprintf(stderr,"setsockopt\n");
            exit(-11);
        }
        if(bind(fd,p->ai_addr,p->ai_addrlen)==-1){
            perror("bind()");
            close(fd);
            continue;
        }
        break;
    }
    if(!p){
        std::cout<<"list samapt"<<std::endl;
        exit(-1);
    }
    freeaddrinfo(res);
    if(listen(fd,BACKLOG)==-1){
        perror("listen");
        exit(-1);
    }
    return fd;
}

void add_to_pfds(std::vector<struct pollfd> &pfds, int &count, int fd){
    struct pollfd nfd;
    nfd.fd = fd;
    nfd.revents = 0;
    nfd.events = POLLIN;  //ready to read
    if(size(pfds)<=count){
        pfds.push_back(nfd);
    }
    else{
        pfds[count] = nfd;
    }
    count++;
}

void del_from_pfds(std::vector<struct pollfd> &pfds, int &count, int i){
    pfds[i] = pfds[count-1];
    count--;
}

void handle_new_connection(std::vector<struct pollfd> &pfds, int &count, int listner){
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen;
    int newfd;
    char ip[INET6_ADDRSTRLEN];
    addrlen = sizeof remoteaddr;
    newfd = accept(listner,(struct sockaddr*)&remoteaddr,&addrlen);
    if(newfd ==-1){
        perror("accept()");
        return;
    }
    add_to_pfds(pfds,count,newfd);
    std::cout<<"Pollserver got connection from "<<inet_ntop2(&remoteaddr, ip,sizeof ip)<<newfd<<std::endl;

}

void handle_client_data(int listner, std::vector<struct pollfd> &pfds, int &count, int &fd_i){
    char buff[256];
    int nbytes = recv(pfds[fd_i].fd,&buff, sizeof buff, 0);
    int sender_fd = pfds[fd_i].fd;
    if(nbytes<=0){
        if(nbytes<0){
            perror("recv()");
        }
        else{
            printf("socket %d closed hung up\n", sender_fd);
        }
        close(pfds[fd_i].fd);
        del_from_pfds(pfds,count,fd_i);
        fd_i--;
    }
    else{
        printf("pollserver: recv from fd %d: %s",sender_fd,buff);
        for(int i=0;i<count;i++){
            int dest_fd = pfds[i].fd;
            if(dest_fd!=sender_fd && dest_fd!=listner){
                if(send(dest_fd,buff,nbytes,0)==-1){
                    perror("send");
                }
            }
        }
    }
    
}

void process_connections(int listner, std::vector<struct pollfd> &pfds, int &count){
    for(int i=0;i<count;i++){
        if(pfds[i].revents & (POLLIN|POLLHUP)){
            if(pfds[i].fd ==listner){
                handle_new_connection(pfds,count,listner);
            }
            else{
                handle_client_data(listner,pfds,count,i);
            }
        }
    }
}

int main(){
    int listner = get_listner_socket();
    int count =0;
    if(listner==-1){
        fprintf(stderr, "error getting listening socket\n");
        exit(1);
    }
    std::vector<struct pollfd> pfds;
    pfds.push_back({listner,POLLIN});
    count =1;
    puts("pollserver: waiting for connections...");
    for(;;){
        int pollcount = poll(pfds.data(),count,-1);
        if(pollcount==-1){
            perror("poll");
            exit(1);
        }
        process_connections(listner,pfds,count);
    }
}