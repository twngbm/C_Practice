#ifndef __SOCKET__
#define __SOCKET__

extern int opt;

extern int CreateSocketClient(struct sockaddr_in *addr,int port,char *address);

extern int CreateSocketServer(struct sockaddr_in *addr,int port,char *address,int maxS);

extern int Socket();

extern int Setsockopt(int fd,int *opt);

extern int Bind(int fd,struct sockaddr_in *addr);

extern int Listen(int fd,int maxConnection);

extern int Accept(int fd,struct sockaddr_in *addr);

#endif