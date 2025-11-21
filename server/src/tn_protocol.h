/*
  EXTERNAL.H
 */

#ifndef _TN_PROTOCOL_H_
#define _TN_PROTOCOL_H_

// #include"config.h"
#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<strings.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include <sys/param.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <errno.h>
#include<math.h>
#include<time.h>

#include <netinet/in.h>
#include <inttypes.h>

#include "conv_table_to_20140.h"

/* prototypes */

int tn_card_write(int client_sockfd, int send_card[8][15], int protocol_version); 
int tn_int_write(int client_sockfd, int i , int protocol_version);
int tn_card_read(int client_sockfd, int read_card[8][15], int protocol_version); 

#endif 

