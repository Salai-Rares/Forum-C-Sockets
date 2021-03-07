#!/bin/sh

gcc -c stdsock.c
gcc -c server.c
gcc -c client.c
gcc -c persistance.c
gcc -o server server.o stdsock.o persistance.o -pthread
gcc -o client client.o stdsock.o -pthread
