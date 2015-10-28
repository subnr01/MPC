#include "tcp.h"


#define MAX 4

#include <opencv2/highgui/highgui_c.h>
using namespace std;
using namespace cv;

#include <unistd.h>

//int     is_data_ready = 0;

void  quit(string msg, int retval, int sockfd);

#define checkResults(string, val) {             \
if (val) {                                     \
printf("Failed with %d at %s", val, string); \
exit(1);                                     \
}                                              \
}



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
    pthread_t thread_master;
    //namedWindow("stream_server1", CV_WINDOW_AUTOSIZE);
    void *status;
    int ret = 0;
    
    /* accept a request from a client */
    if ((connectSock = accept(sockfd, (sockaddr*)&clientAddr, &clientAddrLen)) == -1) {
        printf("accept() failed");
    }else{
        cout << "-->Receiving image from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << "..." << endl;
    }
    
    /* run the Master thread */
    if (pthread_create(&thread_master, NULL, masterThread, &connectSock)) {
        printf("\n pthread_create failed.\n");
    }
    int key;
    
    while(key != 'q') {
        key = waitKey(10);
        int err = getpeername(sockfd, (struct sockaddr *) &clientAddr, &clientAddrLen);
        if ( !err) {
            cout<<"\n client has disconnected \n";
            break;
        }
        //usleep(100);
       
    }
    ret = pthread_cancel(thread_master);
    checkResults("pthread_cancel()\n", ret);
    ret = pthread_join(thread_master, &status);
    if (status != PTHREAD_CANCELED) {
        cout<<endl<<"Master thread returned unexpected result!\n";
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

void cleanupHandler(void *parm) {
    int ret = 0;
    void *status = NULL;
    pthread_t *threads = (pthread_t *)parm;
    //destroyWindow("stream_server");
    printf("Inside cancellation cleanup handler\n");
    for (int i = 0; i < MAX; i++) {
        ret = pthread_cancel(threads[i]);
        checkResults("pthread_cancel()\n", ret);
        ret = pthread_join(threads[i], &status);
        if (status != PTHREAD_CANCELED) {
            cout<<endl<<"Thread "<<i<<" returned unexpected result!\n";
        }
    }
    quit("NULL", 0, 0);
  }


void* masterThread(void* arg)
{
    int height;
    int width;
    int *connectSock = (int*)arg;
    
    bool threads_created = false;
    width = 640;
    height = 480;
    cv::Mat img;
    img = cv::Mat::zeros( height, width, CV_8UC1);
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
    pthread_cleanup_push(cleanupHandler, threads);
    
    
    
    /* start receiving images */
    while(true)
    {
        
        for (int i = 0; i < imgSize; i += bytes) {
            if ((bytes = recv(*connectSock, sockData +i, imgSize  - i, 0)) == -1) {
                img.release();
                quit("recv failed", 1, *connectSock);
            }
        }
        
        //cout<<endl<<"bytes = "<<bytes;
        
        /* convert the received data to OpenCV's Mat format, thread safe */
        for (int i = 0;  i < img.rows; i++) {
            for (int j = 0; j < img.cols; j++) {
                (img.row(i)).col(j) = (uchar)sockData[((img.cols)*i)+j];
            }
        }
        pthread_testcancel();
        //client_info.block_queue.push(img);
        client_info.connectfd = *connectSock;
        
         
        if (!threads_created) {
            for (int i = 0; i < MAX; i++) {
                client_info.actionType = i;
                cout<<"\n creating threads "<<i;
                if (pthread_create(&threads[i], NULL, slaveThread, &client_info)) {
                    printf("\n pthread_create failed.\n");
                    
                }
            }
            threads_created = true;
        }
        imshow("stream_server", img);
        waitKey(30);
        memset(sockData, 0x0, sizeof(sockData));
    }
    
    pthread_cleanup_pop(0);
    
    /* no, take a rest for a while */
    usleep(1000);
    //img.release();
    
}

void processMatching (client_info_t *client_info) {
    client_info = client_info;
    processVideo(client_info);
    //printf("\n hello \n");
}




void *slaveThread(void *arg) {
    std::cout<<endl<<"in slave thread";
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    client_info_t *client_info = (client_info_t*)arg;
    //`processMatching(client_info);
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

