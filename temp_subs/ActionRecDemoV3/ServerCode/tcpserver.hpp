//
//  tcpserver.hpp
//  ServerCode
//
//  Created by Subs on 10/16/15.
//  Copyright © 2015 Subs. All rights reserved.
//

#ifndef tcpserver_hpp
#define tcpserver_hpp

#include <stdio.h>

#define PORT 1234

int tcpServer(int port);
void *servlet(void *arg);
void sendresult(int *childfd);
void receiveVideo(int *childfd);

#endif /* tcpserver_hpp */
