//
//  tcp.h
//  ServerCode
//
//  Created by Subs on 10/17/15.
//  Copyright © 2015 Subs. All rights reserved.
//

#ifndef tcp_h
#define tcp_h



int establishTCPconnection(int);
void* masterThread(void* arg);
void* slaveThread(void *);
void processTCPConnection(int);



#endif /* tcp_h */
