//
//  main.cpp
//  OpenCVCamera
//
//  Created by Fabian Töpfer on 30.01.21.
//  Copyright © 2021 baniaf. All rights reserved.
//
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/objdetect.hpp>

#include <iostream>

using namespace std;
using namespace cv;


float getMotionDifference(Mat& prevMat, Mat& currentMat){
    Mat diff;
    cv::subtract(prevMat,currentMat,diff);

    return cv::mean(diff)[0];
}


int main(int argc, const char * argv[]) {
    
    namedWindow("Motion");

    VideoCapture VideoStream(0);

    if(!VideoStream.isOpened()){
        printf("Error cannot open camera");
        return 1;
    };

    Mat ReferenceFrame;
    Mat OldReferenceFrame;
    
    Mat GrayFrame;
    Mat linearPolarFrame;
    
    Mat diff;
    
    VideoStream >> ReferenceFrame;
    cvtColor(ReferenceFrame, OldReferenceFrame, COLOR_BGR2GRAY);
        
    Mat rgbchannel[3];
    
    Mat mergedArray;

    do{ 
        VideoStream >> ReferenceFrame;
        cvtColor(ReferenceFrame, GrayFrame, COLOR_BGR2GRAY);
      
        float diff = getMotionDifference(OldReferenceFrame,GrayFrame);
        
        cv::split(ReferenceFrame, rgbchannel);
        
        
        double b[2][3] = {{1,0,(diff - 1) * 20}, {0,1,0}};
        Mat translationMatBlue = Mat(2,3,CV_64FC1,b);
        
        double r[2][3] = {{1,0,-(diff - 1)* 20}, {0,1,0}};
        Mat translationMatRed = Mat(2,3,CV_64FC1,r);
        
        warpAffine(rgbchannel[0] , rgbchannel[0] , translationMatBlue,rgbchannel[0].size(), INTER_LINEAR, BORDER_TRANSPARENT);
        warpAffine(rgbchannel[2] , rgbchannel[2] , translationMatRed,rgbchannel[2].size(), INTER_LINEAR, BORDER_TRANSPARENT);

        cv::merge(rgbchannel,3, mergedArray);
        
        imshow("Motion",mergedArray);

        OldReferenceFrame = GrayFrame.clone();

    }while(waitKey(30) < 0);

    return 0;
}
