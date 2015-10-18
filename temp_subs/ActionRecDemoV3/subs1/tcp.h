//
//  tcp.h
//  ServerCode
//
//  Created by Subs on 10/17/15.
//  Copyright © 2015 Subs. All rights reserved.
//

#ifndef tcp_h
#define tcp_h


#define PORT 1234

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#include <string>
#include <pthread.h>
#include <queue>


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "blocking_queue.h"


using namespace cv;

typedef struct client_info {
    int connectfd;
    myqueue<Mat> block_queue;
    int actionType;
}client_info_t;

int processVideo(client_info_t*);

int establishTCPconnection(int);
void* masterThread(void* arg);
void* slaveThread(void *);
void processTCPConnection(int);
int tcpServer(int port);


#endif /* tcp_h */
