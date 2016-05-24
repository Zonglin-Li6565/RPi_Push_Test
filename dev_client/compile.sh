#!/bin/sh
gcc -c handle.c
gcc -c server.c
gcc server.o handle.o -o server
gcc client.c -o client
