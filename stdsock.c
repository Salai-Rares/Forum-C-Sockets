#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void std_err(int val,char *mes){
    if(val<0){
        perror(mes);
        exit(EXIT_FAILURE);
    }
}

