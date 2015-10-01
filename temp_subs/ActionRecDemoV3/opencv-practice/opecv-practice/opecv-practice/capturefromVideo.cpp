//
//  capturefromVideo.cpp
//  opecv-practice
//
//  Created by Subs on 9/24/15.
//  Copyright © 2015 Subs. All rights reserved.
//

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;

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
        sprintf(filename,"/Users/admin/images/test_%d.png",i);
        cvtColor(frame, gray, CV_BGR2GRAY);
        Mat img_bw = gray > 128;
        imwrite(filename, img_bw);
        i++;
        key = cv::waitKey(10);
        if ( key == 27) break;
    }
}

