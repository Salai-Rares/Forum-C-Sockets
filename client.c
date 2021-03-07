#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>


#include "stdsock.h"

void* writeForum(void *arg){
    int fd=*((int*)arg);
    char buffer[SIZE];

    for(;;){
        fflush(stdin);
        fgets(buffer,SIZE,stdin);
        buffer[strlen(buffer)-1]='\0';
        std_err(send(fd,buffer,SIZE,0),"writeForm err send()");
        if(!strcmp(buffer,"exit")){
          close(fd);
          break;
        }
    }
    exit(0);
}



int main(){
    int sock;
    struct sockaddr_in address; 
    char buffer[SIZE];
    char username[LEN],password[LEN];
    sock=socket(AF_INET,SOCK_STREAM,0);
    std_err(sock,"socket failure");
    address.sin_family=AF_INET;
    address.sin_port=htons(PORT);
    address.sin_addr.s_addr=INADDR_ANY;
    std_err(connect(sock,(struct sockaddr*)&address,sizeof(address)),"conect failure");
    printf("username: ");
    fflush(stdin);
    fgets(username,LEN,stdin);
    username[strlen(username)-1]='\0';
    std_err(send(sock,username,LEN,0),"send() err1");
    std_err(recv(sock,&buffer,SIZE,0),"recv() err1");
    printf("%s",buffer);
    fflush(stdin);
    fgets(password,LEN,stdin);
    password[strlen(password)-1]='\0';
    std_err(send(sock,&password,strlen(password),0),"send() err2");
    memset(&buffer,0,SIZE);
    std_err(recv(sock,&buffer,SIZE,0),"recv() err2");
    puts(buffer);
    if(!strcmp(buffer,"Wrong Password !")){
        close(sock);
        return 1;
    }
    char aux[SIZE]="has joined the chatroom";
    std_err(send(sock,aux,sizeof(aux),0),"send() err3");
    printf("Type 'exit' to leave chatroom\nType 'online' to find out who is online\n");
    pthread_t var;
    
    pthread_create(&var,NULL,writeForum,(void*)&sock);
    for(;;){
        memset(&buffer,0,SIZE);
        std_err(recv(sock,&buffer,SIZE,0),"recv() err");
        printf("%s",buffer);
        fflush(stdout);
        sleep(1);
    }
    close(sock);
    return 0;
}