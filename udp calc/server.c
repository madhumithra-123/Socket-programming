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
    int sockfd,n;
    char buffer[256]={'0'};
    struct sockaddr_in serv_addr,cli_addr;
    socklen_t clilen;
    sockfd=socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd<0) error("Socket error");
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=INADDR_ANY;
    serv_addr.sin_port=htons(PORT);
    if(bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0)
        error("Bind failed");
    while(1){
        clilen=sizeof(cli_addr);
        n=recvfrom(sockfd,buffer,255,0,(struct sockaddr*)&cli_addr,&clilen);
        if(n<0) continue;
        int a,c;
        char b;
        if(sscanf(buffer,"%d %c %d",&a,&b,&c)==3){
            int res;
            switch(b){
                case '+': res=a+b; break;
                case '-': res=a-b; break;
                case '*': res=a*b; break;
                case '/': if(b!=0) res=a/b; break;
                default: res=-1;
            }
            if(res==-1){
                buffer[0]='E',buffer[1]='R',buffer[2]='R',buffer[3]='O',buffer[4]='R';
                buffer[5]='\0';
            }
            int i=0,k=res;
            while(k){
                i++;
                k/=10;
            }
            buffer[i]='\0';
            for(int j=i-1;j>-1;j--){
                buffer[j]='0'+res%10;
                res/=10;
            }
        }
    }
    sendto(sockfd,buffer,strlen(buffer),0,(struct sockaddr*)&cli_addr,&clilen);
    close(sockfd);
    return 0;
}