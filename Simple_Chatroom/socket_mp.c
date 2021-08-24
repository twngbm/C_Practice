#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#define PORT 8080

void getchild(int signum){
  pid_t p;
  while(waitpid(-1,NULL,WNOHANG)){
    ;
  }
  return;
}

int main(){
  signal(SIGCHLD,getchild);
  printf("wait/IP:\n");
  char type[16]={'\0'};
  scanf("%s",type);
  if(strcmp(type,"wait")==0){
    //server
    printf("server PPID:%d\n",getpid());
    int server_fd;
    int opt=1;
    struct sockaddr_in *address=(struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
    int addrlen=sizeof(*address);
    server_fd=socket(AF_INET,SOCK_STREAM,0);
    if(!server_fd){
      perror("Socket fd failed");
      exit(EXIT_FAILURE);
    }
    if(setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT,&opt,sizeof(opt))){
      perror("set socket opt failed");
      exit(EXIT_FAILURE);
    }
    address->sin_family=AF_INET;
    address->sin_addr.s_addr=INADDR_ANY;
    address->sin_port=htons(PORT);
    if(bind(server_fd,(const struct sockaddr*)address,addrlen)<0){
      perror("bind failed.");
      exit(EXIT_FAILURE);
    }
    if(listen(server_fd,3)<0){
      perror("listen failed.");
      exit(EXIT_FAILURE);
    }
    while(1){
      int new_socket=accept(server_fd,(struct sockaddr*)address,&addrlen);
      if(new_socket<0){
        perror("Accept new socket failed.");
        exit(EXIT_FAILURE);
      }
      printf("Socket %d connected.\n",new_socket);
      pid_t pid=fork();
      if(!pid){
        printf("Socker Server %d CPID:%d.\n",new_socket,getpid());
        char buffer[1000]={'\0'};
        int valread=0;
        while(1){
          memset(buffer,0,1000);
          if(read(new_socket,buffer,1000)){
            printf("%d:%s\n",new_socket,buffer);
            send(new_socket,"ECHO",5,0);
          }
          else{
            printf("Socket %d closed CPID:%d.\n",new_socket,getpid());
            close(new_socket);
            exit(EXIT_SUCCESS);
          }
        }
      }
    }
    
    
    return 0;
  }
  else{
    //client
    printf("Client PID:%d\n",getpid());
    int client_fd;
    struct sockaddr_in *serv_addr=(struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
    client_fd=socket(AF_INET,SOCK_STREAM,0);
    if(client_fd<0){
      perror("Socket FD Create Failed");
      exit(EXIT_FAILURE);
    }
    serv_addr->sin_family=AF_INET;
    serv_addr->sin_port=htons(PORT);
    if(inet_pton(AF_INET,type,&serv_addr->sin_addr)<=0){
      perror("Address not supported.");
      exit(EXIT_FAILURE);
    }
    if(connect(client_fd,(const struct sockaddr*)serv_addr,sizeof(*serv_addr))<0){
      perror("Connection failed.");
      exit(EXIT_FAILURE);
    }
    while(1){
      char input[1024]={'\0'};
      char recv[1000]={'\0'};
      scanf("%s",input);
      if(strcmp(input,"bye")==0){
        close(client_fd);
        exit(EXIT_SUCCESS);
      }
      send(client_fd,input,strlen(input),0);
      memset(recv,0,1000);
      read(client_fd,recv,1000);
      printf("%s\n",recv);
    }
    
    return 0;
  }
  return 0;
}