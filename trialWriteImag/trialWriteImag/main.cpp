//
//  main.cpp
//  trialWriteImag
//
//  Created by Priyanka Kulkarni on 10/2/15.
//  Copyright © 2015 Priyanka Kulkarni. All rights reserved.
//

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdlib.h>
#include <list>
#include <time.h>
#include <string>

using namespace cv;
using namespace std;

const double TIME_CALC_FRAMES = 1;



#include "opencv2/highgui/highgui.hpp"
#include <iostream>

using namespace cv;
using namespace std;


/*

#include <iostream>

// Include standard OpenCV headers
#include "opencv/cv.h"
#include "opencv/highgui.h"

// Include Boost headers for system time and threading
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/thread.hpp"

using namespace std;
using namespace boost::posix_time;
using namespace cv;

// Code for capture thread
void captureFunc(Mat *frame, cv::VideoCapture *capture){
    //loop infinitely
    for(;;){
        //capture from webcame to Mat frame
        (*capture) >> (*frame);
    }
}

//main
int main (int argc, char *argv[])
{

    
    // initialize capture on default source
    cv::VideoCapture capture(0);
    
    // set framerate to record and capture at
    int framerate = 15;
    
    // Get the properties from the camera
    double width = capture.get(CV_CAP_PROP_FRAME_WIDTH);
    double height = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
    
    // print camera frame size
    cout << "Camera properties\n";
    cout << "width = " << width << endl <<"height = "<< height << endl;
    
    // Create a matrix to keep the retrieved frame
    Mat frame;
    
    // Create the video writer
    VideoWriter video("capture.avi",CV_FOURCC('D','I','V','X'), framerate, cvSize((int)width,(int)height) );
    
    // initialize initial timestamps
    nextFrameTimestamp = microsec_clock::local_time();
    currentFrameTimestamp = nextFrameTimestamp;
    td = (currentFrameTimestamp - nextFrameTimestamp);
    
    // start thread to begin capture and populate Mat frame
    boost::thread captureThread(captureFunc, &frame, &capture);
    
    // loop infinitely
    for(;;)
    {
        
        // wait for X microseconds until 1second/framerate time has passed after previous frame write
        while(td.total_microseconds() < 1000000/framerate){
            //determine current elapsed time
            currentFrameTimestamp = microsec_clock::local_time();
            td = (currentFrameTimestamp - nextFrameTimestamp);
        }
        
        //	 determine time at start of write
        initialLoopTimestamp = microsec_clock::local_time();
        
        // Save frame to video
        video << frame;
        
        //write previous and current frame timestamp to console
        cout << nextFrameTimestamp << " " << currentFrameTimestamp << " ";
        
        // add 1second/framerate time for next loop pause
        nextFrameTimestamp = nextFrameTimestamp + microsec(1000000/framerate);
        
        // reset time_duration so while loop engages
        td = (currentFrameTimestamp - nextFrameTimestamp);
        
        //determine and print out delay in ms, should be less than 1000/FPS
        //occasionally, if delay is larger than said value, correction will occur
        //if delay is consistently larger than said value, then CPU is not powerful
        // enough to capture/decompress/record/compress that fast.
        finalLoopTimestamp = microsec_clock::local_time();
        td1 = (finalLoopTimestamp - initialLoopTimestamp);
        delayFound = td1.total_milliseconds();
        cout << delayFound << endl;
        
        //output will be in following format
        //[TIMESTAMP OF PREVIOUS FRAME] [TIMESTAMP OF NEW FRAME] [TIME DELAY OF WRITING]
        
    }
    
    // Exit
    return 0;
}
*/

/*


int main(void)
{
    VideoWriter outputVideo; // For writing the video
    
    int width = 640; // Declare width here
    int height = 480; // Declare height here
    Size S = Size(width, height); // Declare Size structure
    
    // Open up the video for writing
    const string filename = "/Users/priyankakulkarni/Documents/cap_frames/out.avi"; // Declare name of file here
    
    // Declare FourCC code
    int fourcc = CV_FOURCC('A','V','C','1');
    
    // Declare FPS here
    int fps = 3;
    outputVideo.open(filename, fourcc, fps, S);
}
*/

/*

int main(int argc, char* argv[])
{

    
    VideoCapture cap = cv2.VideoCapture(0);
    
// Define the codec and create VideoWriter object
    fourcc = cv2.VideoWriter_fourcc(*'XVID')
    out = cv2.VideoWriter('output.avi',fourcc, 20.0, (640,480))
    
    while(cap.isOpened()):
        ret, frame = cap.read()
        if ret==True:
            frame = cv2.flip(frame,0)
            
// write the flipped frame
            out.write(frame)
            
            cv2.imshow('frame',frame)
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break
                else:
                    break
                    
// Release everything if job is finished
                    cap.release()
                    out.release()
                    cv2.destroyAllWindows()
            }
 
 */

/*
    
    IplImage *frame;
    CvCapture *capture = cvCaptureFromFile("/Users/priyankakulkarni/Documents/Project/opencv-3.0-2.0/samples/cpp/tutorial_code/HighGUI/video-input-psnr-ssim/video/Megamind.avi");
    if ( capture == NULL )
    {
        std::cout << "video not ok" << std::endl;
    }
    else
    {
        std::cout << "video ok" << std::endl;
        cvSetCaptureProperty(capture, CV_CAP_PROP_FPS, 4.0);
        double videoFPS = cvGetCaptureProperty( capture, CV_CAP_PROP_FPS );
        std::cout << videoFPS << std::endl;

       // frame = cvQueryFrame(capture);
        while(1)
        {
            cvShowImage("Video", frame);
            cvWaitKey(1000/videoFPS);
            
            if(waitKey(30) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
            {
                cout << "esc key is pressed by user" << endl;
                break;
            }

        }
        cvDestroyWindow("Video");
        cvReleaseImage(&frame);
        cvReleaseCapture(&capture);
    }
}
 */

    /*
    
    String path ="/Users/priyankakulkarni/Documents/cap_frames/out.avi";
    VideoCapture cap(path); // open the video file for reading
    
    if ( !cap.isOpened() )  // if not success, exit program
    {
        cout << "Cannot open the video file" << endl;
        return -1;
    }
    
    //cap.set(CV_CAP_PROP_POS_MSEC, 300); //start the video at 300ms
    
    double fps = cap.get(CV_CAP_PROP_FPS); //get the frames per seconds of the video
    
    cout << "Frame per seconds : " << fps << endl;
    
    namedWindow("MyVideo",CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"
    
    while(1)
    {
        Mat frame;
        
        bool bSuccess = cap.read(frame); // read a new frame from video
        
        if (!bSuccess) //if not success, break loop
        {
            cout << "Cannot read the frame from video file" << endl;
            break;
        }
        
        imshow("MyVideo", frame); //show the frame in "MyVideo" window
        
        if(waitKey(30) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            break;
        }
    }
    
    return 0;
    
}
     */

/*
int main(){
    
    VideoCapture vcap(0);
    if(!vcap.isOpened()){
        cout << "Error opening video stream or file" << endl;
        return -1;
    }
    
    int frame_width=   vcap.get(CV_CAP_PROP_FRAME_WIDTH);
    int frame_height=   vcap.get(CV_CAP_PROP_FRAME_HEIGHT);
    VideoWriter video("/Users/priyankakulkarni/Documents/cap_frames/out.avi",CV_FOURCC('M','J','P','G'),10, Size(frame_width,frame_height),true);
    
    for(;;){
        
        Mat frame;
        vcap >> frame;
        video.write(frame);
        imshow( "Frame", frame );
        char c = (char)waitKey(33);
        if( c == 27 ) break;
    }
    return 0;
}
     */

/*

int main(int argc, char* argv[])
{
    VideoCapture cap(0); // open the video camera no. 0
    
    if (!cap.isOpened())  // if not success, exit program
    {
        cout << "Cannot open the video cam" << endl;
        return -1;
    }
    
    double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
    double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video
    double count = cap.get(CV_CAP_PROP_FPS);
    
    cout << "Frame size : " << dWidth << " x " << dHeight << endl;
    cout << "Frame count : " << count << endl;
    
    

    
    namedWindow("MyVideo",CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"
    
    while (1)
    {
        Mat frame;
        
        bool bSuccess = cap.read(frame); // read a new frame from video
        
        if (!bSuccess) //if not success, break loop
        {
            cout << "Cannot read a frame from video stream" << endl;
            break;
        }
        
        imshow("MyVideo", frame); //show the frame in "MyVideo" window
        
        if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            break; 
        }
    }
    return 0;
    
}


*/


/*

int main( int argc, char** argv )
{
   
 
    printf("* Entered \n");
    
  //  CvCapture *capture = cvCaptureFromAVI(argv[1]);
    CvCapture *capture = cvCreateCameraCapture(0);
    if(!capture)
    {
        printf("!!! cvCreateCameraCapture failed \n");
        return -1;
    }
    
  //  double fps = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
   // printf("* FPS: %f\n", fps);
    
    
    
    IplImage* frame = NULL;
    int frame_number = 0;
    char key = 0;
    
    // prepare some timing stuff
    std::list<clock_t> clocktimes(TIME_CALC_FRAMES);
    clock_t curTime, frontTime;
    float avg_delay, fps;
    int last_delay = 0;
    
    float desired_delay = 1.0f / 12.0f;
        int clockFactor = 1;
    
    clock_t prevTime = clock();
    curTime = clock();
    int index = 0;
    
    while (key != 'q')
    {
        prevTime = curTime;
        curTime = clock();
        frontTime = clocktimes.front();
        
        clocktimes.pop_front();
        clocktimes.push_back(curTime);
        
        avg_delay = (float)(curTime - prevTime) / (float)(TIME_CALC_FRAMES * clockFactor); // * CLOCKS_PER_SEC);
        fps = 1.0f / avg_delay;
        //std::cout << "avg_delay: " << avg_delay << std::endl;
        std::cout << "fps: " << fps << std::endl;
        
        // get frame
        frame = cvQueryFrame(capture);
        if (!frame)
        {
            printf("!!! cvQueryFrame failed: no frame\n");
            break;
        }
       
        /*
        
      //  char filename[100];
        std::string filename="/Users/priyankakulkarni/Documents/cap_frames/";
        filename = filename+"frame_";
        
       // char frame_id[30];
        std::string frame_id = std::to_string(frame_number);
       // itoa(frame_number, frame_id, 10);
        filename = filename+frame_id;
        filename = filename+".jpg";
    //    strcat(filename, frame_id);
     //   strcat(filename, ".jpg");
        
        printf("* Saving: %s\n", filename.c_str());
        
        if (!cvSaveImage(filename.c_str(), frame))
        {
            printf("!!! cvSaveImage failed\n");
            break;
        }
        
        frame_number++;
        
        // quit when user press 'q'
        //key = cvWaitKey(1000 / fps);
         
         */
  /*
    }
    
    // free resources
    cvReleaseCapture(&capture);
    
    return 0;
}
*/


int main( int argc, char** argv ) {
    cv::namedWindow( "Example2_10", cv::WINDOW_AUTOSIZE );
    cv::VideoCapture cap;
    cv::Mat frame;
    char key;
    int i = 0;
    char filename[80];
    
    if(argc == 1) {
        cap.open(0);
    } else {
        cap.open(argv[1]);
    }
    
    if(!cap.isOpened() ) {
        std::cerr << "Couldn't open capture." << std::endl;
        return -1;
    }
    
    while(1)
    {
        cap.read ( frame);
        if( frame.empty()) break;
        imshow("video", frame);
        Mat gray;
      //  cv::Rect myROI(400, 200, 80, 140);
        //frame = frame(myROI);
        sprintf(filename,"/Users/priyankakulkarni/Desktop/Trial10/test_%d.png",i);
        cvtColor(frame, gray, CV_BGR2GRAY);
        Mat img_bw = gray > 128;
        cv::imwrite(filename, img_bw);
        i++;
        key = cv::waitKey(10);
        if ( key == 27) break;
    }
    
}
