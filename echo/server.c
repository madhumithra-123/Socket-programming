#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#define PORT 8080
void error(const char* msg){
    perror(msg);
    exit(1);
}
int main(){
    int sockfd,n,newsockfd;
    char buffer[256];
    socklen_t clilen;
    struct sockaddr_in serv_addr,cli_addr;
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0) error("Socket fail");
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=INADDR_ANY;
    serv_addr.sin_port=htons(PORT);
    if(bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0)
        error("Bind failed");
     clilen=sizeof(cli_addr);
     listen(sockfd,5);
     newsockfd=accept(sockfd,(struct sock_addr*)&cli_addr,&clilen);
     if(newsockfd<0) error("accept failed");
     printf("Accept done\n");
     read(newsockfd,buffer,255);
     printf("Received from client : %s",buffer);
     write(newsockfd,buffer,strlen(buffer));
     printf("Echo sent");
     close(newsockfd);
     close(sockfd);
     return 0;
}