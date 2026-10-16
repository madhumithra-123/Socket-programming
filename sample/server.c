#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<string.h>
#define PORT 8080
void error(const char* msg){
    perror(msg);
    exit(1);
}
int main(){
    int socketfd,newsockfd,n;
    char buffer[255];
    struct sockaddr_in serv_addr,cli_addr;
    socklen_t clilen;
    socketfd=socket(AF_INET,SOCK_STREAM,0);
    if(socketfd<0) error("Error opening socket");
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=INADDR_ANY;
    serv_addr.sin_port=htons(PORT);
    if(bind(socketfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0) 
        error("Binding error");
    listen(socketfd,5);
    clilen=sizeof(cli_addr);
    newsockfd=accept(socketfd,(struct sockaddr*)&cli_addr,&clilen);
    if(newsockfd<0) error("Error on accept");
    while(1){
        int n=recv(newsockfd,buffer,255,0);
        if(n<0) error("error in read");
        printf("Client : %s",buffer);
        fgets(buffer,255,stdin);
        send(newsockfd,buffer,strlen(buffer),0);
        int i=strncmp("Bye",buffer,3);
        if(i==0) break;
    }
    close(newsockfd);
    close(socketfd);
    return 0;
}
