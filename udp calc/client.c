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
    int sockfd;
    struct sockaddr_in serv_addr;
    char buffer[256];
    sockfd=socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd<0) error ("Socket fail");
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(PORT);
    if(inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr)<0)
        error("address fail");
    fgets(buffer,255,stdin);
    int s=strlen(buffer);
    sendto(sockfd,buffer,s,0,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    recvfrom(sockfd,buffer,255,0,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    printf("Reversed : %s",buffer);
    close(sockfd);
    return 0;
}