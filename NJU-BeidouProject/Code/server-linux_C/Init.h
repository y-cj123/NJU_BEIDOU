#ifndef INIT_H
#define INIT_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

extern int upload_signal;
extern char upload_filename[30];

extern int download_signal;
extern int command_lenth;
extern unsigned char download_command[1024];

unsigned char char2xchar(unsigned char *c,int n);

#endif
