//
//  tcpserver.cpp
//  ServerCode
//
//  Created by Subs on 10/16/15.
//  Copyright © 2015 Subs. All rights reserved.
//

#include "tcpserver.hpp"
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define DEBUG 1

#define BUFSIZE 1024


/*
 * error - wrapper for perror
 */
void error(const char *msg) {
    perror(msg);
    exit(1);
}

int tcpServer(int port) {
    int parentfd; /* parent socket */
    int childfd; /* child socket */
    int portno; /* port to listen on */
    int clientlen; /* byte size of client's address */
    struct sockaddr_in serveraddr; /* server's addr */
    struct sockaddr_in clientaddr; /* client addr */
    struct hostent *hostp; /* client host info */
    
    char *hostaddrp; /* dotted decimal host addr string */
    int optval; /* flag value for setsockopt */
    pthread_t child;
    
    portno = port;
    
    
    if (DEBUG) {
        printf("\n Creating the server socket");
    }
    parentfd = socket(AF_INET, SOCK_STREAM, 0);
    if (parentfd < 0) {
        error("ERROR opening socket");
       
    }
    
    optval = 1;
    if (DEBUG) {
        printf("\n Setting socket options");
    }
    setsockopt(parentfd, SOL_SOCKET, SO_REUSEADDR,
               (const void *)&optval , sizeof(int));
    
    
    if (DEBUG) {
        printf("\n Build Server IP address");
    }
    
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)portno);
   
    if (DEBUG) {
        printf("\n Binding server IP");
    }
    if (bind(parentfd, (struct sockaddr *) &serveraddr,
             sizeof(serveraddr)) < 0) {
            error("ERROR on binding");
    }
    
    if (DEBUG) {
        printf("\n Listening for server connections");
    }
    
    if (listen(parentfd, 5) < 0) /* allow 5 requests to queue up */
        error("ERROR on listen");
    
    
    
    clientlen = sizeof(clientaddr);
    
    while (1) {
        
        /*
         * accept: wait for a 
         * connection request
         */
        if (DEBUG) {
            printf("\n Waiting for a client to connect\n");
        }
        childfd = accept(parentfd, (struct sockaddr *) &clientaddr, (socklen_t *) &clientlen);
        if (childfd < 0) {
            error("ERROR on accept");
        }
        
        /*
         * gethostbyaddr: determine who 
         * sent the message
         */
        hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,
                              sizeof(clientaddr.sin_addr.s_addr), AF_INET);
        
        if (hostp == NULL) {
            error("ERROR on gethostbyaddr");
        }
        
        hostaddrp = inet_ntoa(clientaddr.sin_addr);
        if (hostaddrp == NULL) {
            error("ERROR on inet_ntoa\n");
        }
        
        printf("\n server established connection with %s (%s)\n",
               hostp->h_name, hostaddrp);
        
      
        pthread_create(&child, 0, servlet, &childfd);
        pthread_detach(child);
        
    }
    
}

void *servlet(void *arg)
{
    int *childfd = (int*)arg;
    receiveVideo(childfd);
    sendresult(childfd);
    close(*childfd);
    return 0;
}


void receiveVideo(int *childfd)
{
    char buf[BUFSIZE];
    ssize_t n;
    
    
    if (n < 0) {
        error("ERROR reading from socket");
    }
    
    if (DEBUG) {
        printf("server received %zd bytes: %s", n, buf);
    }

    bzero(buf, BUFSIZE);
    
    n = read(*childfd, buf, BUFSIZE);
}



void sendresult(int *childfd) {
    char buf[BUFSIZE];
    ssize_t n;
    n = write(*childfd, "Received", strlen(buf));
    if (n < 0) {
        error("ERROR writing to socket");
    }

}
