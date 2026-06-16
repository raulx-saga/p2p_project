#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc, char* argv[]){
    struct addrinfo hints, *res, *p;
    int status; 
    char ipstr[INET6_ADDRSTRLEN];
    if(argc!=2){
        fprintf(stderr,"usage: showip hostname\n");
        return 1;
    }

    // fill in hints 
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    // call the addr
    if((status = getaddrinfo(argv[1],NULL,&hints,&res))!=0){
        fprintf(stderr,"getaddrinfo: %s\n",gai_strerror(status));
        return 2;
    }

    // we now have the structs we need. let's retrieve the data. 
    printf("IP addresses for %s:\n",argv[1]);

    //for loop over the obtained linked list. 
    for(p = res;p!=NULL;p=p->ai_next){
        //define the required pointers 
        void * addr;
        const char * ipver;
        struct sockaddr_in * ipv4;
        struct sockaddr_in6 * ipv6;

        //check if ipv4 or ipv6
        if(p->ai_family == AF_INET){
            //use ipv4
            ipver ="IPv4";
            ipv4 = (struct sockaddr_in*)p->ai_addr;
            addr = &(ipv4->sin_addr);
        }
        else{
            ipver="IPv6";
            ipv6 = (struct sockaddr_in6*)p->ai_addr;
            addr = &(ipv6->sin6_addr);
        }
        inet_ntop(p->ai_family,addr,ipstr,INET6_ADDRSTRLEN);
        printf("%s:%s\n",ipver,ipstr);
        
    }
    freeaddrinfo(res);
    return 0;
}