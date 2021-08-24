#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <pthread.h>
//#include <sys/select.h>
#include <sys/epoll.h>
#include "Socket.h"
#define PORT 8080
#define MAXCONNECT 100

int FDbuffer[MAXCONNECT];

int AddFDbuffer(int fd){
  int temp=fd;
  while(FDbuffer[temp%MAXCONNECT]){
    temp++;
    if((temp-fd)>MAXCONNECT){
      send(fd,"BUFFER FULL",12,0);
      close(fd);
      perror("BUFFER FULL");
      return -1;
    }
  }
  FDbuffer[temp%MAXCONNECT]=fd;
  return 0;
}

void RemoveFDbuffer(int fd){
  for(int i=0;i<MAXCONNECT;i++){
    if(FDbuffer[i]==fd){
      FDbuffer[i]=0;
      return;
    }
  }
}

void NotifyFDbuffer(int fd,char *message){
  for(int i=0;i<MAXCONNECT;i++){
    int target;
    target=FDbuffer[i];
    if(target!=0&&target!=fd){
      dprintf(target,"%d:%s",fd,message);
    }
  }
  return;
  
}

typedef struct __connectionInfo__{
  int socketFD;
} socketInfo;

void *connectionHandler(void *vargp){
  int new_socket=((socketInfo*)vargp)->socketFD;
  printf("Socker Server %d CPID:%d.\n",new_socket,getpid());
  char buffer[1024]={'\0'};
  while(1){
    memset(buffer,0,1024);
    if(read(new_socket,buffer,1024)){
      printf("%d : %s\n",new_socket,buffer);
      NotifyFDbuffer(new_socket,buffer);
    }
    else{
      RemoveFDbuffer(new_socket);
      close(new_socket);
      free((socketInfo*)vargp);
      return NULL;
    }
  }

}

int main(){
  printf("wait/IP:\n");
  char type[16]={'\0'};
  scanf("%s",type);
  if(strcmp(type,"wait")==0){
    //server
    int server_fd;
    struct sockaddr_in *address=(struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
    server_fd=CreateSocketServer(address,PORT,"0.0.0.0",MAXCONNECT);
    memset(FDbuffer,0,sizeof(int)*MAXCONNECT);
    while(1){
      int new_socket=Accept(server_fd,address);
      if(AddFDbuffer(new_socket)<0){
        continue;
      }
      socketInfo *data=(socketInfo*)malloc(sizeof(socketInfo));
      data->socketFD=new_socket;
      printf("Socket %d connected.\n",new_socket);
      pthread_t threadID;
      pthread_create(&threadID,NULL,connectionHandler,(void*)data);
      pthread_detach(threadID);
    }  
    return 0;
  }
  else{
    //client
    char input[1024]={'\0'};
    char recv[1024]={'\0'};
    printf("Client PID:%d\n",getpid());
    int client_fd;
    struct sockaddr_in *serv_addr=(struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
    client_fd=CreateSocketClient(serv_addr,PORT,type);
    int epollFD=epoll_create(2);
    struct epoll_event *epolldata=(struct epoll_event*)malloc(sizeof(struct epoll_event));
    struct epoll_event event[2];
    epolldata->events=EPOLLIN;
    epolldata->data.fd=0;
    epoll_ctl(epollFD,EPOLL_CTL_ADD,0,epolldata);
    epolldata->data.fd=client_fd;
    epoll_ctl(epollFD,EPOLL_CTL_ADD,client_fd,epolldata);
    while(1){ 
      int number=epoll_wait(epollFD,event,2,-1);
      if(number<=0){
        perror("epoll error.");
        continue;
      }
      for(int i=0;i<number;i++){
        int currentFD=event[i].data.fd;
        if(currentFD==0){
          scanf("%s",input);
          send(client_fd,input,strlen(input),0);
          if(strcmp(input,"bye")==0){
            close(client_fd);
            exit(EXIT_SUCCESS);
          }
        }
        else if(currentFD==client_fd){
          memset(recv,0,1024);
          if(read(client_fd,recv,1024)){
            printf("%s\n",recv);
          }
          else{
            close(client_fd);
            printf("Connection Closed.\n");
            return EXIT_SUCCESS;
          }
          
        }
      }
    }
  }
  return 0;
}