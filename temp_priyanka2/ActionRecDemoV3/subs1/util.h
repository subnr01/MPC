//
//  util.h
//  ClientCode
//
//  Created by Subs on 10/27/15.
//  Copyright © 2015 Subs. All rights reserved.
//

#ifndef util_h
#define util_h


#define PORT 9000

using namespace std;




void* sendData(void* arg);
void* receiveDataThread(void*);
int establishTCPConnection(char *serverip);
int establishUDP(char *serverip);
void  quit(string msg, int retval);




#endif /* util_h */
