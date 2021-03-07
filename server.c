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
#include "persistance.h"

#define MAX_CLIENTS 10

typedef struct {
   int fd;
   char nume[LEN];
}cl_t;


cl_t sockets[MAX_CLIENTS];
static _Atomic int cnt=0;
pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;

void sendMessage(char *username,char *mes){
    char new_mes[SIZE+LEN];
    sprintf(new_mes,"%s: %s\n",username,mes);
    pthread_mutex_lock(&lock);
    for(int i=0;i<MAX_CLIENTS;i++){
       if(sockets[i].fd!=-1 && strcmp(username,sockets[i].nume)!=0)
          std_err(send(sockets[i].fd,new_mes,SIZE+LEN,0),"send() err5");
    }
    pthread_mutex_unlock(&lock);
}


void* client_handler(void *arg){
    int fd=*((int*)arg);
    char buffer[SIZE];
    char mes[SIZE];
    cnt++;
    memset(&buffer,0,SIZE);
    memset(&mes,0,SIZE);
    std_err(recv(fd,&buffer,SIZE,0),"recv() err1");
    int temp=userAvailable(buffer);
    if(temp<0){
      strcpy(mes,"SignUp starting\npassword: ");
      std_err(send(fd,&mes,sizeof(mes),0),"send() err1");
      memset(&mes,0,SIZE);
      std_err(recv(fd,&mes,LEN,0),"recv() err2");
      pthread_mutex_lock(&lock);
      addPerson(buffer,mes);
      pthread_mutex_unlock(&lock);
      memset(&mes,0,SIZE);
      strcpy(mes,"Welocome to Chatroom");
      std_err(send(fd,&mes,sizeof(mes),0),"send() err3");
    }else{
      strcpy(mes,"Username already in use, if you want to login, enter your password\npassword: ");
      std_err(send(fd,&mes,sizeof(mes),0),"send() err2");
      memset(&mes,0,SIZE);
      std_err(recv(fd,&mes,LEN,0),"recv() err3");
      if(!strcmp(mes,tab[temp].password)){
          strcpy(mes,"Welocome to Chatroom");
          std_err(send(fd,&mes,sizeof(mes),0),"send() err3");
      }else{
          strcpy(mes,"Wrong Password !");
          std_err(send(fd,&mes,sizeof(mes),0),"send() err4");
          close(fd);
          return NULL;
      }
    }
    int i;
    pthread_mutex_lock(&lock);
    for(i=0;i<MAX_CLIENTS;i++){
        if(sockets[i].fd==-1){
            sockets[i].fd=fd;
            strcpy(sockets[i].nume,buffer);
            cnt++;
            break;
        }
    }
    pthread_mutex_unlock(&lock);
    
    for(;;){
        memset(&buffer,0,SIZE);
        std_err(recv(fd,&buffer,SIZE,0),"recv() err ");
        if(!strcmp(buffer,"exit")){
            close(fd);
            char aux[SIZE];
            sprintf(aux,"has left the chatroom");
            int j;
            for(j=0;j<MAX_CLIENTS;j++){
                if(sockets[j].fd==fd){
                   sendMessage(sockets[j].nume,aux);
                   sockets[j].fd=-1;
                }
            }
            cnt--;
            break;
        }
        if(!strcmp(buffer,"online")){
            char aux[(MAX_CLIENTS+3)*LEN+30]="These people are online:\n";
            for(int i=0;i<MAX_CLIENTS;i++){
                if(sockets[i].fd>0){
                    strcat(aux,"\t> ");
                    strcat(aux,sockets[i].nume);
                    strcat(aux,"\n");
                }
            }
            std_err(send(fd,aux,strlen(aux),0),"send() err5");
            continue;
        }
        sendMessage(sockets[i].nume,buffer);
        sleep(1);
    }
    pthread_detach(pthread_self());
    return NULL;
}

int main(){
   int master_socket,sock;
   struct sockaddr_in address;
   int addrlen=sizeof(address);
   int opt=1;
   master_socket=socket(AF_INET,SOCK_STREAM,0);
   std_err(master_socket,"master failed");
   if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons(PORT); 
    std_err(bind(master_socket,(struct sockaddr*)&address,addrlen),"bind failed");
    std_err(listen(master_socket,5),"listen failed");
    for(int i=0;i<MAX_CLIENTS;i++)
       sockets[i].fd=-1;
    pthread_t var;
    populate();
    puts("Server is waiting for conection");
    for(;;){
        if(cnt<MAX_CLIENTS){
            sock=accept(master_socket,(struct sockaddr*)&address,(socklen_t*)&addrlen);
            std_err(sock,"accept failure");
            std_err(pthread_create(&var,NULL,client_handler,(void*)&sock),"thread_failure");
        }
    }
}
