#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "persistance.h"

void populate(){
    countdb=0;
    FILE *f=fopen(path,"rb");
    while(fread(&tab[countdb++],sizeof(pers),1,f));
    fclose(f);
}

void store(){
    FILE *f=fopen(path,"wb");
    for(int i=0;i<countdb;i++)
      fwrite(&tab[i],sizeof(pers),1,f);
    fclose(f);  
}

int userAvailable(char *username){
    for(int i=0;i<countdb;i++)
       if(!strcmp(tab[i].name,username))
          return i;
    return -1;      
}

void addPerson(char *username,char *password){
    FILE *f=fopen(path,"ab");
    strcpy(tab[countdb].name,username);
    strcpy(tab[countdb].password,password);
    fwrite(&tab[countdb],sizeof(pers),1,f);
    countdb++;
    fclose(f);
}