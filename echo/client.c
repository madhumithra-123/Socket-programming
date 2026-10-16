#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
# define PORT 8080
void error(const char *msg){
    perror(msg);
    exit(1);
}
int main(){
    int sockfd,n;
    char buffer[256];
    struct sockaddr_in serv_addr;
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0) error("Socket failed");
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(PORT);
    if(inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr)<0)
        error("address fail");
    if(connect(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0)
        error("Connectio failed");
    printf("Enter message : ");
    fgets(buffer,255,stdin);
    int s=strlen(buffer);
    write(sockfd,buffer,s);
    read(sockfd,buffer,s);
    printf("Echoed back : %s",buffer);
    close(sockfd);
    return 0;
}