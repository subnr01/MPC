//
//  DisplayVideo.cpp
//  opecv-practice
//
//  Created by Subs on 9/19/15.
//  Copyright (c) 2015 Subs. All rights reserved.
//

/* Program to display an video */


#include "opencv2/highgui/highgui.hpp" 
#include "opencv2/imgproc/imgproc.hpp"
#include<iostream>

/*
int main( int argc, char** argv ) {
    cv::namedWindow( "Example3", cv::WINDOW_AUTOSIZE );
    cv::VideoCapture cap;
    cap.open( std::string("/Users/admin/images/hpvm2004demo.wmv") );
    cv::Mat frame;
    
    while(1){
        cap>>frame;
        if( !frame.data ) {
            std::cout<<"hello";
            break;
        }
        cv::imshow( "Example3", frame );
        if( cv::waitKey(33) >= 0 ) break;
    }
    return 0;
}
*/

#include "opencv2/highgui/highgui.hpp"
#include <iostream>


using namespace cv;
using namespace std;

int main4(int argc, char* argv[])
{

    VideoCapture cap("/Users/admin/images/Jurassic.World.2015.avi"); // open the video file for reading
    
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