#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "util.h"
#include "blocking_queue.h"
#include <pthread.h>

#define MAX 4
#include<opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>

using namespace std;
using namespace cv;

void* slaveThread1(void*);

int main(int argc, const char * argv[])
{
    int sock;
    
    bool threads_created = false;
    
    struct sockaddr_in addr;
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);
    
    
    double weight = 800;
    double height = 600;
    char windowName[] = "Server Camera";
    int receiveSize = 65*1024;
    char buff[receiveSize];
    int c;
    int received;
    vector<uchar> ibuff;
    client_info_t client_info[4];
    pthread_t threads[MAX];
    
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9000);
    addr.sin_addr.s_addr = INADDR_ANY;
    //addr.sin_len = sizeof(addr);
    
    
    int n = 1024 * 1024;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &n, sizeof(n)) == -1) {
        // deal with failure, or ignore if you can live with the default size
    }
    
    if (setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &n, sizeof(n)) == -1) {
        // deal with failure, or ignore if you can live with the default size
    }
    
    if( (::bind(sock, (struct sockaddr *)&addr, sizeof(addr)))<0){
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    //cvNamedWindow( windowName, CV_WINDOW_AUTOSIZE );
    memset(buff, 0, 5);;
    printf("\nWaiting for clients to connect\n");
    printf("\nWaiting for clients to connect\n");
    printf("\nWaiting for clients to connect\n");
    while(1){
        //printf("Recieved\n");
        memset(buff, 0, receiveSize);
        received = recvfrom(sock, buff, receiveSize, 0, (struct sockaddr *)&client_addr, &addrlen);
        
        if(received != -1)
        {
            for(int i = 0; i < received; i++)
            {
                ibuff.push_back((uchar)buff[i]);
            }
            
            cv::Mat image = cv::Mat(weight, height, CV_8UC3);
            image = imdecode( Mat(ibuff), CV_LOAD_IMAGE_COLOR);
            //cv::imshow(windowName, image);
            //client_info.block_queue.push(image);
            
            for ( int i = 0 ; i < MAX; i++) {
                
                client_info[i].mydeque.push_front(image);
                
            }
            /*
            client_info.udp_client_addr = client_addr;
            client_info.mydeque.push_front(image);
            client_info.connectfd = sock;
            */
             
             //processVideo(&client_info) ;
            if (!threads_created) {
                for (int i = 0; i < MAX; i++) {
                    client_info[i].actionType = i;
                    cout<<"\n creating threads "<<i;
                    client_info[i].udp_client_addr = client_addr;
                    client_info[i].connectfd = sock;
                    //client_info_t th_client_info = client_info[i];
                    if (pthread_create(&threads[i], NULL, slaveThread1, &client_info[i])) {
                        printf("\n pthread_create failed.\n");
                        
                    }
                }
                threads_created = true;
            }
            
            ibuff.clear();
        } else
        {
            perror("sock");
        }
        
        //c = cvWaitKey (1);
        if (c == 'q')
            break;
    }
    
    close(sock);
    return 0;
}


void *slaveThread1(void *arg) {
    std::cout<<endl<<"in slave thread "<<pthread_self()<<endl;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    client_info_t *client_info = (client_info_t*)arg;
    processVideo(client_info) ;
    return 0;
}