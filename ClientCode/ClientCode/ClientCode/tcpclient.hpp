//
//  tcpclient.hpp
//  ClientCode
//
//  Created by Subs on 10/16/15.
//  Copyright © 2015 Subs. All rights reserved.
//

#ifndef tcpclient_hpp
#define tcpclient_hpp

#define PORT 1234

#include <stdio.h>

int startTCPClient(char *servername, int port);
void sendvideo(int);



#endif /* tcpclient_hpp */
