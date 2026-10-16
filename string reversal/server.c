#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#define PORT 8080
void error(const char* msg){
    perror(msg);
    exit(1);
}
int main(){
    int sockfd,newsockfd;
    char buffer[256]={'0'};
    struct sockaddr_in serv_addr,cli_addr;
    socklen_t clilen;
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0) error("Socket error");
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=INADDR_ANY;
    serv_addr.sin_port=htons(PORT);
    if(bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0)
        error("Bind failed");
    listen(sockfd,5);
    clilen=sizeof(cli_addr);
    newsockfd=accept(sockfd,(struct sockaddr*)&cli_addr,&clilen);
    if(newsockfd<0) error("Accept failed");
    read(newsockfd,buffer,255);
    int i=0;
    while(i<255&&buffer[i]!='\0') i++;
    char rev[i];
    for(int j=0;j<i;j++) rev[j]=buffer[i-j-1];
    write(newsockfd,rev,i);
    close(newsockfd);
    close(sockfd);
    return 0;
}