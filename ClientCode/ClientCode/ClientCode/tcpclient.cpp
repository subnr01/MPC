//
//  tcpclient.cpp
//  ClientCode
//
//  Created by Subs on 10/16/15.
//  Copyright © 2015 Subs. All rights reserved.
//


/*****************************************************************************/
/*** tcpclient.c                                                           ***/
/***                                                                       ***/
/*** Demonstrate an TCP client.                                            ***/
/*****************************************************************************/

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <resolv.h>
#include <netdb.h>
#include "tcpclient.hpp"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>



void error(const char *msg) {
    perror(msg);
    exit(1);
}


void startTCPClient(const char *servername, int port)
{
    struct hostent* hostptr;
    struct sockaddr_in addr;
    int sockfd = 0;
  
    hostptr = gethostbyname(servername);
  
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if ( sockfd < 0 ) {
        error("socket");
    }
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    memcpy((void *) &(addr.sin_addr), (void *) hostptr->h_addr, hostptr->h_length);
    
    
    
    if ( connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == 0)
    {
        sendvideo(sockfd);
    }
    else {
        error("connect");
    }
}

void sendvideo(int sockfd) {
    char s[200];
    FILE *fp = fdopen(sockfd, "r+");
    
    fprintf(fp, "\nsending to server hello\n");
    fflush(fp);
    while ( fgets(s, sizeof(s), fp) != 0 )
        fputs(s, stdout);
    printf("\n");
    fclose(fp);

}