//
//  main.cpp
//  opecv-practice
//
//  Created by Subs on 9/18/15.
//  Copyright (c) 2015 Subs. All rights reserved.
//
#include <opencv2/opencv.hpp> //Include file for every supported OpenCV function
int main2( int argc, char** argv ) {
    cv::Mat img = cv::imread(argv[1],-1);
    if( img.empty() ) return -1;
    cv::namedWindow( "Example1", cv::WINDOW_AUTOSIZE );
    cv::imshow( "Example1", img );
    cv::waitKey( 0 );
    cv::destroyWindow( "Example1" );
    return 0;
}