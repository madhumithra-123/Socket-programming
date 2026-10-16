#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#define PORT 8080
void error(const char* msg){
    perror(msg);
    exit(1);
}
int main(){
    int sockfd,n;
    struct sockaddr_in serv_addr;
    char buffer[255];
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0) error("error opening socket");
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(PORT);
    if(inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr)<=0)
        error("invalid address");
    if(connect(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0)
        error("Connection failed");
    while(1){
        printf("Client : ");
        fgets(buffer,255,stdin);
        write(sockfd,buffer,strlen(buffer));
        n=read(sockfd,buffer,255);
        if(n<0) error("Error on read");
        printf("Server : %s",buffer);
        int i=strncmp(buffer,"Bye",3);
        if(i==0) break;
    }
    close(sockfd);
    return 0;
}