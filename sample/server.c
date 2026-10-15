#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<string.h>
void error(const char* msg){
    perror(msg);
    exit(1);
}
int main(int argc, char*argv[]){
    if(argc<2) {
        printf(stderr,"port no. not provided, Terminated\n");
        exit(1);
    }
    int socketfd,newsockfd,portno,n;
    char buffer[255];
    struct sockaddr_in serv_addr,cli_addr;
    socklen_t clilen;
    socketfd=socket(AF_INET,SOCK_STREAM,0);
    if(socketfd<0) error("Error opening socket");
    portno=atoi(argv[1]);
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=INADDR_ANY;
    serv_addr.sin_port=htons(portno);
    if(bind(socketfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0) 
        error("Binding error");
    listen(socketfd,5);
    clilen=sizeof(cli_addr);
    newsockfd=accept(socketfd,(struct sockaddr*)&cli_addr,&clilen);
    if(newsockfd<0) error("Error on accept");
    while(1){
        int n=read(newsockfd,buffer,255);
        if(n<0) error("error in read");
        printf("Client : %s",buffer);
        fgets(buffer,255,stdin);
        write(newsockfd,buffer,strlen(buffer));
        int i=strncmp("Bye",buffer,3);
        if(i==0) break;
    }
    close(newsockfd);
    close(socketfd);
    return 0;
}
