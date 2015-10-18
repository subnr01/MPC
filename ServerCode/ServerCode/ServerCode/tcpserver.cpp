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
#include "tcp.h"

#define MAX 5
#define NUM_TEMPLATE_IMG 32

using namespace std;
using namespace cv;


int     is_data_ready = 0;

void  quit(string msg, int retval, int sockfd);


typedef struct client_info {
    int connectfd;
    myqueue<Mat> block_queue;
    int start;
    int end;
}client_info_t;


int tcpServer(int port)
{
    int sockfd = establishTCPconnection(port);
    cout << "-->Waiting for TCP connection on port " << port << " ...\n\n";
    
    while (1)
    {
        processTCPConnection(sockfd);
    }
    return 0;
}


void processTCPConnection(int sockfd) {
    
    struct sockaddr_in clientAddr;
    socklen_t      clientAddrLen = sizeof(clientAddr);
    int connectSock;
    pthread_t thread_s;
    namedWindow("stream_server1", CV_WINDOW_AUTOSIZE);
    
    /* accept a request from a client */
    if ((connectSock = accept(sockfd, (sockaddr*)&clientAddr, &clientAddrLen)) == -1) {
        printf("accept() failed");
    }else{
        cout << "-->Receiving image from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << "..." << endl;
    }
    
    /* run the Master thread */
    if (pthread_create(&thread_s, NULL, masterThread, &connectSock)) {
        printf("\n pthread_create failed.\n");
    }
    int key;
    
    while(key != 'q') {
        key = waitKey(10);
    }
}

int establishTCPconnection(int port) {
    int     listenSock;
    struct  sockaddr_in   serverAddr;
    
    if ((listenSock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        quit("socket() failed.", 1, listenSock);
    }
    
    serverAddr.sin_family = PF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(port);
    
    int ret = ::bind(listenSock, (struct sockaddr *) &serverAddr,
                     sizeof(serverAddr));
    
    if (ret < 0) {
        quit("bind() failed", 1, listenSock);
    }
    
    
    if (listen(listenSock, 5) == -1) {
        quit("listen() failed.", 1, listenSock);
    }
    return listenSock;
    
}



void* masterThread(void* arg)
{
    int height;
    int width;
    int *connectSock = (int*)arg;
    
    bool threads_created = false;
    width = 640;
    height = 480;
    Mat img;
    img = Mat::zeros( height, width, CV_8UC1);
    int  imgSize = img.total() * img.elemSize();
    
    int  bytes=0;
    
    char sockData[imgSize];
    memset(sockData, 0x0, sizeof(sockData));
    
    
    /* make this thread cancellable using pthread_cancel() */
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    pthread_t  threads[MAX];
    client_info_t client_info;
    namedWindow("stream_server", CV_WINDOW_AUTOSIZE);
    
    
    
    
    /* start receiving images */
    while(true)
    {
        
        for (int i = 0; i < imgSize; i += bytes) {
            if ((bytes = recv(*connectSock, sockData +i, imgSize  - i, 0)) == -1) {
                img.release();
                quit("recv failed", 1, *connectSock);
            }
        }
        
        cout<<endl<<"bytes = "<<bytes;
        
        /* convert the received data to OpenCV's Mat format, thread safe */
        for (int i = 0;  i < img.rows; i++) {
            for (int j = 0; j < img.cols; j++) {
                (img.row(i)).col(j) = (uchar)sockData[((img.cols)*i)+j];
            }
        }
        
        client_info.block_queue.push(img);
        client_info.connectfd = *connectSock;
        
         
        if (!threads_created) {
            for (int i = 0; i < MAX; i++) {
                client_info.start = i;
                client_info.end = i + 2;
                if (pthread_create(&threads[i], NULL, slaveThread, &client_info)) {
                    printf("\n pthread_create failed.\n");
                    
                }
                threads_created = true;
            }
        }
        
        
        is_data_ready = 1;
        

        imshow("stream_server", img);
        waitKey(30);
        memset(sockData, 0x0, sizeof(sockData));
        is_data_ready = 1;
        //pthread_mutex_unlock(&gmutex);
        
    }
    
    /* have we terminated yet? */
    pthread_testcancel();
    
    /* no, take a rest for a while */
    usleep(1000);
    //destroyWindow("stream_server");
    //img.release();
    quit("NULL", 0, *connectSock);
    
}

void processMatching (client_info_t *client_info) {
    client_info = client_info;
    //printf("\n hello \n");
}

void *slaveThread(void *arg) {
    client_info_t *client_info = (client_info_t*)arg;
    processMatching(client_info);
    return 0;
}




void quit(string msg, int retval, int sockfd)
{
    if (retval == 0) {
        cout << (msg == "NULL" ? "" : msg) << "\n" <<endl;
    } else {
        cerr << (msg == "NULL" ? "" : msg) << "\n" <<endl;
    }
    
    if (sockfd) {
        close(sockfd);
    }
    
    
   
    exit(retval);
}

