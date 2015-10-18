//
//  main.cpp
//  ServerCode
//
//  Created by Subs on 10/16/15.
//  Copyright © 2015 Subs. All rights reserved.
//

#include <iostream>
#include "tcp.h"

using namespace cv;

int main(int argc, const char * argv[]) {
   
    // insert code here...
    tcpServer(PORT);
    //udpServer(PORT);
    return 0;
}
