//
//  pra2.cpp
//  opecv-practice
//
//  Created by Subs on 9/19/15.
//  Copyright (c) 2015 Subs. All rights reserved.
//

/* Program showing an image */

#include <opencv2/opencv.hpp> //Include file for every supported OpenCV function
int mainw( int argc, char** argv ) {
    cv::Mat img = cv::imread("/Users/admin/images/a15.jpg",-1);
    std::cout<<"hello";
    if( img.empty() ) return -1;
    cv::namedWindow( "Example1", cv::WINDOW_AUTOSIZE );
    cv::imshow( "Example1", img );
    cv::waitKey( 0 );
    cv::destroyWindow( "Example1" );
    return 0;
}
