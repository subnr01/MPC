//
//  util.h
//  subs1
//
//  Created by Subs on 10/23/15.
//  Copyright © 2015 Subs. All rights reserved.
//

#ifndef util_h
#define util_h

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

#define PORT 9000
#define MAX_THREADS 4


using namespace cv;

typedef struct client_info {
    struct sockaddr_in udp_client_addr;
    int connectfd;
    std::deque<Mat> mydeque;
    int actionType;
}client_info_t;

struct UDPrequest{
    int sock;
    socklen_t fromlen;
    struct sockaddr_in from;
    char *buf;
};


int establishUDPConnection(int port);
int establishTCPconnection(int port);
void* slaveThread(void*);
void* masterThread(void*);
int processVideo(client_info_t*);


#endif /* util_h */
