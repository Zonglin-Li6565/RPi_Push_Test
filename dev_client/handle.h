#ifndef _HANDLE_H
#define _HANDLE_H 

#include <string.h>
#include <stdio.h>
#include <wiringPi.h>
#include <pthread.h>

int handle_init();

int handle(char* command);

#endif
