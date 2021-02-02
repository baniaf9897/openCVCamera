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
    Mat blurPrev,blurCurrent,diff;
    
    
    cv::GaussianBlur(prevMat, blurPrev, Size(21,21), 0);
    cv::GaussianBlur(currentMat, blurCurrent, Size(21,21), 0);
    
    cv::absdiff(blurPrev,blurCurrent,diff);
    cv::threshold(diff, diff, 25, 255, THRESH_BINARY);


    return cv::mean(diff)[0];
}


int main(int argc, const char * argv[]) {
    
    namedWindow("Motion");
    namedWindow("Diff");

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
        std::cout<<diff<<std::endl;
        cv::split(ReferenceFrame, rgbchannel);
        
        
        double b[2][3] = {{1,0,(diff) * 20}, {0,1,0}};
        Mat translationMatBlue = Mat(2,3,CV_64FC1,b);
        
        double r[2][3] = {{1,0,-(diff)* 20}, {0,1,0}};
        Mat translationMatRed = Mat(2,3,CV_64FC1,r);
        
        warpAffine(rgbchannel[0] , rgbchannel[0] , translationMatBlue,rgbchannel[0].size(), INTER_LINEAR, BORDER_TRANSPARENT);
        warpAffine(rgbchannel[2] , rgbchannel[2] , translationMatRed,rgbchannel[2].size(), INTER_LINEAR, BORDER_TRANSPARENT);

        cv::merge(rgbchannel,3, mergedArray);
        
        Mat diffMat;
        cv::subtract(GrayFrame,OldReferenceFrame,diffMat); 
        imshow("Motion",mergedArray);
        imshow("Diff",  diffMat);

        OldReferenceFrame = GrayFrame.clone();

    }while(waitKey(30) < 0);
    

    return 0;
}
