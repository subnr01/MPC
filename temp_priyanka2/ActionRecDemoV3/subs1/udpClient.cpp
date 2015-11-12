#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "util.h"
#include "IntImage.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <pthread.h>
#include <fstream>

using namespace std;
using namespace cv;


#define DEBUG 1
#define TCP 0
#define UDP 1



void *receiveData(void *);

struct sockaddr_in addr;

// Draw rectangle function

void drawRect(IntImage* dest, int cx, int cy, int w, int h, int r, int g, int b)
{
    int* rc = dest->getChannel(0);
    int* gc = dest->getChannel(1);
    int* bc = dest->getChannel(2);
    int iw = dest->width();
    int ih = dest->height();
    
    // left edge
    for(int y = cy; y < std::min(ih, cy + h); ++y)
    {
        rc[(y * iw) + cx] = r;
        gc[(y * iw) + cx] = g;
        bc[(y * iw) + cx] = b;
    }
    
    // right edge
    if(cx + w < iw)
    {
        for(int y = cy; y < std::min(ih, cy + h); ++y)
        {
            rc[(y * iw) + cx + w] = r;
            gc[(y * iw) + cx + w] = g;
            bc[(y * iw) + cx + w] = b;
        }
    }
    
    // top edge
    for(int x = cx; x < std::min(iw, cx + w); ++x)
    {
        rc[(cy * iw) + x] = r;
        gc[(cy * iw) + x] = g;
        bc[(cy * iw) + x] = b;
    }
    
    // bottom edge
    if(cy + h < ih)
    {
        for(int x = cx; x < std::min(iw, cx + w); ++x)
        {
            rc[((cy + h) * iw) + x] = r;
            gc[((cy + h) * iw) + x] = g;
            bc[((cy + h) * iw) + x] = b;
        }
    }
}


int main()
{
    int sockfd;
    int send_len;
    
    char *serverip = "127.0.0.1";
  //  char *serverip = "128.2.213.227";
    
    //char *server_ip = "128.2.213.222";
    pthread_t receive_thread;
    double width = 640;
    double height = 480;
    IplImage *frame = 0;
    cv::Mat Mimg;
    vector<uchar> ibuff;
    vector<int> param = vector<int>(2);
    char windowName[] = "Client Camera";
    int sendSize = 65535;
    char buff[sendSize];
    
    //sockfd = establishUDP(server_ip);
    
    if (UDP) {
        if (DEBUG) {
            cout<<"\n Establishing UDP connection \n";
        }
        sockfd = establishUDP(serverip);
    }
    else if (TCP) {
        if (DEBUG) {
            cout<<"\n Establishing TCP connection \n";
        }
        sockfd = establishTCPConnection(serverip);
    }
    
    /*
     * run the receiving thread
     */
    if (pthread_create(&receive_thread, NULL, receiveData, &sockfd)) {
        printf("\n--> pthread_create failed.");
    }
    
    
    CvCapture *capture = cvCreateCameraCapture(0);
    cvSetCaptureProperty (capture, CV_CAP_PROP_FRAME_WIDTH, width);
    cvSetCaptureProperty (capture, CV_CAP_PROP_FRAME_HEIGHT, height);
    
    cvSetCaptureProperty(capture, CV_CAP_PROP_FPS, 3.0);
    
    cvNamedWindow (windowName, CV_WINDOW_AUTOSIZE);
    
    
    /*
     * Perform jpeg compression
     */
    frame = cvQueryFrame (capture);
    Mimg = cvarrToMat(frame);
    param[0] = CV_IMWRITE_JPEG_QUALITY;
    param[1] = 50;
    
    
    imencode(".jpg", Mimg, ibuff, param);
    if (DEBUG) {
        cout<<"coded file size(jpg)"<<ibuff.size()<<endl;
    }
    
    while (1) {
        frame = cvQueryFrame (capture);
        Mimg = cvarrToMat(frame);
        
        if(capture)
             cout<<"uh oh :"<<endl;
        
    

        
        imencode(".jpg", Mimg, ibuff, param);
        for (int i = 0; i < ibuff.size(); i++)
        {
            buff[i]=ibuff[i];
        }
        if (UDP) {
            if (DEBUG) {
                cout<<"\n Sending Image \n";
            }
            send_len = sendto(sockfd, buff, ibuff.size(), 0, (struct sockaddr *)&addr, sizeof(addr));
        }
        if (send_len == -1)
        {
            perror("socket");
            exit(1);
        } else
        {
            printf("buffer size %lu \n", ibuff.size());
        }
        
        //rectangle(Mimg,Point(20,0),Point(170,65),Scalar(255,255,0),1,8,0);
        imshow(windowName, Mimg);
        
        
        int process_width = 180;
        int process_height = 144;
        int display_width = 320;
        int display_height = 240;
        
        int searchX = 20;
        int searchY = 0;
        int searchW = 150;
        int searchH = 65;
        
        IplImage* ipl_Mimg_pointer;
        IntImage* src_Mimg = new IntImage(process_width, process_height, 3);
        IntImage* dest_Mimg = new IntImage(process_width, process_height, 3);
        IplImage* display_temp = cvCreateImage(cvSize(process_width, process_height), IPL_DEPTH_8U, 3);
        IplImage* resizedSrcFrame = cvCreateImage(cvSize(display_width, display_height), IPL_DEPTH_8U, 3);
        IplImage* resizedPFrame = cvCreateImage(cvSize(process_width, process_height), IPL_DEPTH_8U, 3);
        
        IplImage ipl_Mimg = Mimg;
        
        ipl_Mimg_pointer = &ipl_Mimg;
        
        cvResize(ipl_Mimg_pointer, resizedSrcFrame);
        cvResize(ipl_Mimg_pointer, resizedPFrame);
        
        
        src_Mimg->copy(resizedSrcFrame, true);
        dest_Mimg->copy(resizedPFrame, true);
        
        drawRect(dest_Mimg, searchX, searchY, searchW, searchH, 255, 255, 255);
        
        dest_Mimg->getIplImage(display_temp);
        
        cvShowImage("Template Locations", display_temp);
        
        int c = cvWaitKey (1);
        if (c == 'q')
        {
            break;
        }
    }
    
    /* terminate the receiving thread */
    if (pthread_cancel(receive_thread) ) {
        printf("pthread_cancel failed.");
    }
    
    cvDestroyWindow(windowName);
    close(sockfd);
    return 0;
}



/*
 * thread to receive data
 * continuously
 */

void *receiveData(void *arg)
{
    if (DEBUG) {
        cout<<"\n Receive thread starts"<<endl;
    }
    
    char buff[1024];
    int *sockfd = (int*)arg;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    usleep(10000);
    int i = 0;
    int j = 0;
    
    //while(i < 65,500)
    while (1)
    {
        memset(buff, 0, 1024);
        int received = recvfrom(*sockfd, buff, 1024, 0, NULL,NULL);
        
        cout<<endl<<"Received data"<<endl;

        if ( received != -1)
        {
            buff[received] = '\0';
        }

        cout<<"\n Received buffer data "<<buff<<endl;

        
        if ( !strcmp(buff, "left")) {
            system("/Users/priyankakulkarni/Documents/Project/MPC/keyboard/simulate_keypress_fist_print");
            
        }
        else if ( !strcmp(buff, "right")) {
            system("/Users/priyankakulkarni/Documents/Project/MPC/keyboard/simulate_keypress_palm_print");
            
        }
        
        /*
        if ( i > 65,490 && j < 3 ) {
            i = 0;
            j++;
        }*/
        
        
        i++;
  
        pthread_testcancel();
        
    }
    
    cout<<"\n THREAD EXITING \n";
    
    
}


/*
 * Establish socket connection with
 * the server
 */

//int establishUDP(const char *serverip) {
int establishUDP(char *serverip) {
    
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(serverip);
    
    
    int n = 1024 * 1024;
    
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &n, sizeof(n)) == -1) {
        printf("\n Error in setting socket options for rcvbuff");
        
    }
    
    if (setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &n, sizeof(n)) == -1) {
        printf("\n Error in setting socket options for rcvbuff");
    }
    
    return sockfd;
    
}


int establishTCPConnection(char *server_ip)
{
    
    int clientSock;
    struct  sockaddr_in serverAddr;
    socklen_t serverAddrLen = sizeof(serverAddr);
    
    if ((clientSock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(-1);
    }
    serverAddr.sin_family = PF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(server_ip);
    serverAddr.sin_port = htons(PORT);
    
    if (connect(clientSock, (sockaddr*)&serverAddr, serverAddrLen) < 0) {
        perror("connect failed");
        exit(-1);
    }
    if (DEBUG) {
        cout<<"TCP Connect succeeded"<<endl;
    }
    
    return clientSock;
}