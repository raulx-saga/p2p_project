#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc, char* argv[]){
    char ipstr[INET6_ADDRSTRLEN];
    if(argc!=2){
        fprintf(stderr,"source: Domain not found\n");
        return 2;
    }
    struct addrinfo hints, *res, *p;
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status;
    if((status = getaddrinfo(argv[1],NULL,&hints,&res))!=0){
        fprintf(stderr,"error:getaddrinfo() returned %d",status);
        return status;
    }
    printf("%s : has given following IP's :\n",argv[1]);
    for(p = res; p!=NULL;p=p->ai_next){
        struct sockaddr_in *ipv4;
        struct sockaddr_in6 *ipv6;
        void *addr;
        const char * ipver;

        if(p->ai_family ==AF_INET){
            //ipv4 
            ipver ="IPv4";
            ipv4 = (struct sockaddr_in*)p->ai_addr;
            addr = &(ipv4->sin_addr);
        }
        else if(p->ai_family==AF_INET6){
            //ipv6 
            ipver ="IPv6";
            ipv6 = (struct sockaddr_in6*)p->ai_addr;
            addr = &(ipv6->sin6_addr);
        }
        inet_ntop(p->ai_family,addr,ipstr,INET6_ADDRSTRLEN);
        printf("ipver: %s, ipstr: %s\n",ipver,ipstr);
    }
    return 0;
}