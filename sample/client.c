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
    exit(0);
}
int main(){
    int sockfd,n;
    struct sockaddr_in serv_addr;
    char buffer[255];
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0) error("error opening socket");
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(PORT);
    if(inet_pton(AF_INET,"127.0.0.0",&serv_addr.sin_addr)<=0)
        error("invalid address");
    if(connect(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr)<0))
        error("Connection failed");
    printf("Client : ");
    if(fgets(buffer,255,stdin)!=NULL){
        buffer[strcspn(buffer,"\n")]=0;
        send(sockfd,buffer,strlen(buffer),0);
        n=recv(sockfd,buffer,255,0);
        if(n>0){
            printf("Server : %s",buffer);
        }
    }
    close(sockfd);

}