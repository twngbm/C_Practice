#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include "Socket.h"

int CreateSocketClient(struct sockaddr_in *addr,int port,char *address){
    int fd=Socket();
    addr->sin_family=AF_INET;
    addr->sin_port=htons(port);
    int state=inet_pton(AF_INET,address,&addr->sin_addr);
    if(state<=0){
        perror("Address not supported.");
        exit(EXIT_FAILURE);
    }
    state=connect(fd,(const struct sockaddr*)addr,sizeof(*addr));
    if(state<0){
        perror("Connection Failed.");
        exit(EXIT_FAILURE);
    }
    return fd;
}

int CreateSocketServer(struct sockaddr_in *addr,int port,char *address,int maxS){
    int opt=1;
    
    int fd=Socket();
    
    Setsockopt(fd,&opt);
    
    addr->sin_family=AF_INET;
    addr->sin_port=htons(port);
    addr->sin_addr.s_addr=INADDR_ANY;

    
    Bind(fd,addr);
    Listen(fd,maxS);
    return fd;

}

int Socket(){
    int fd=socket(AF_INET,SOCK_STREAM,0);
    if(!fd){
        perror("Socket fd failed");
        exit(EXIT_FAILURE);
    }
    return fd;
}

int Setsockopt(int fd,int *opt){
    int state=setsockopt(fd,SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT,opt,sizeof(*opt));
    if(state){
        perror("set socket opt failed");
        exit(EXIT_FAILURE);
    }
    return 0;
}

int Bind(int fd,struct sockaddr_in *addr){
    int state=bind(fd,(const struct sockaddr*)addr,sizeof(*addr));
    if(state<0){
        perror("bind failed.");
        exit(EXIT_FAILURE);
    }
    return 0;
}

int Listen(int fd,int maxConnection){
    int state=listen(fd,maxConnection);
    if(state<0){
        perror("listen failed.");
        exit(EXIT_FAILURE);
    }
    return 0;
}

int Accept(int fd,struct sockaddr_in *addr){
    
    unsigned int addrlen=sizeof(*addr);
    int sockets=accept(fd,(struct sockaddr *)addr,&addrlen);
    if(sockets<=0){
        perror("Accept new socket failed.");
        exit(EXIT_FAILURE);
    }
    return sockets;
}