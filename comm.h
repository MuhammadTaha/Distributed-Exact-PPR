#ifndef __COMM__
#define __COMM__

#include <cstring>
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h> 
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <netdb.h>

/*
 * This module is for communication purpose
 *
 * */

void error(const char *msg);
void server(int portno, double * ppv, int userNum, int machineNum);
void client(int portno, char* hostName, double * ppv, int userNum);

#endif
