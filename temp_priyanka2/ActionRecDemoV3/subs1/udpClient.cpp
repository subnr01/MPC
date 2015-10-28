#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <pthread.h>
#include <fstream>
#include "IntImage.h"

using namespace std;
using namespace cv;

int establishUDP();

void *receiveData(void *);

std::ofstream outfile;
struct sockaddr_in addr;


/*
 * thread to inject keyboard
 * event
 */

void *key_event(void *arg)
{
    
    string str = "left";
    
    while(1)
    {
        if(str=="left")
            system("/Users/priyankakulkarni/Documents/Project/MPC/keyboard/simulate_keypress_mac copy");
    }
    


    
    return NULL;
}

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
    int process_width = 180;
    int process_height = 144;
    int display_width = 320;
    int display_height = 240;
    
    int searchX = 20;
    int searchY = 0;
    int searchW = 150;
    int searchH = 65;
    
    int sockfd;
    int send_len;
    
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
    sockfd = establishUDP();
    
    /*
     * run the receiving thread
     
    if (pthread_create(&receive_thread, NULL, receiveData, &sockfd)) {
        printf("\n--> pthread_create failed.");
    }
    */
    
    pthread_t key_event_thread;
    
    if(pthread_create(&key_event_thread, NULL, key_event, NULL))
    {
        printf("\n--> pthread_create failed.");
    }
 
    
    CvCapture *capture = cvCreateCameraCapture(0);
    cvSetCaptureProperty (capture, CV_CAP_PROP_FRAME_WIDTH, width);
    cvSetCaptureProperty (capture, CV_CAP_PROP_FRAME_HEIGHT, height);
    cvNamedWindow (windowName, CV_WINDOW_AUTOSIZE);
    
    /*
     * Perform jpeg compression
     */
    frame = cvQueryFrame (capture);
    Mimg = cvarrToMat(frame);
    param[0] = CV_IMWRITE_JPEG_QUALITY;
    param[1] = 50;
    
    imencode(".jpg", Mimg, ibuff, param);
    cout<<"coded file size(jpg)"<<ibuff.size()<<endl;
    
    while (1) {
        frame = cvQueryFrame (capture);
        Mimg = cvarrToMat(frame);
        
        imencode(".jpg", Mimg, ibuff, param);
        for (int i = 0; i < ibuff.size(); i++)
        {
            buff[i]=ibuff[i];
        }
        
        send_len = sendto(sockfd, buff, ibuff.size(), 0, (struct sockaddr *)&addr, sizeof(addr));
        
        if (send_len == -1)
        {
            perror("socket");
            printf("%lu \n", ibuff.size());
        } else
        {
            printf("%lu \n", ibuff.size());
        }
        
   //     imshow(windowName, Mimg);
        
       
        
     
        
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
    outfile.close();
    close(sockfd);
    return 0;
}



/*
 * thread to receive data
 * continuously
 */

void *receiveData(void *arg)
{
    printf("\n Receive thread starts");
    char buff[1024];
    int *sockfd = (int*)arg;
    outfile.open("output.txt");
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    
    
    while(1)
    {
        memset(buff, 0, 1024);
        int received = recvfrom(*sockfd, buff, 1024, 0, NULL,NULL);
        
        if ( received != -1)
        {
            buff[received] = '\0';
            outfile <<buff;
        }
        pthread_testcancel();
        
    }
}


/*
 * Establish socket connection with
 * the server
 */

//int establishUDP(const char *serverip) {
int establishUDP() {
    
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    
    //char *serverip = "128.2.213.222";
    
    char *serverip = "127.0.0.1";
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9000);
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

