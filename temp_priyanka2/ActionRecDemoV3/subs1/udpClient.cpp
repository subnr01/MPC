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
    std::ofstream outfile;
    outfile.open("/Users/priyankakulkarni/Documents/test.txt", std::ios_base::app);
    
    string str = "left";
    
    while(1)
    {
        if(str=="left")
            system("/Users/priyankakulkarni/Documents/Project/MPC/keyboard/simulate_keypress_mac copy");
    }
    
    outfile.close();

    
    return NULL;
}

int main()
{
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
        
        imshow(windowName, Mimg);
        
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

