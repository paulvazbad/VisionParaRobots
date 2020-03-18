#include "ImageAnalysis.h"
#include <iostream>
#include <string.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace std;
using namespace cv;

ImageAnalysis::ImageAnalysis(Mat &image, string screenName){
  frame = &image;
  this -> screenName = screenName;
  epsilon = 35;
  histImages[0] = Mat( HIST_HEIGHT, HIST_WIDTH, CV_8UC3, Scalar( 0,0,0) );
  histImages[1] = Mat( HIST_HEIGHT, HIST_WIDTH, CV_8UC3, Scalar( 0,0,0) );
  histImages[2] = Mat( HIST_HEIGHT, HIST_WIDTH, CV_8UC3, Scalar( 0,0,0) );
  GenerateRGBHist(*frame);
  namedWindow(screenName);
  setMouseCallback(screenName, onMouse, this);
  cvtColor(*frame, hsvImage, CV_BGR2HSV);
}

void ImageAnalysis::plotHist(){
    int bin_w = cvRound( (double) HIST_WIDTH/HIST_SIZE );
    //Plots main lines of the histogram
    for( int i = 1; i < HIST_SIZE; i++ )
    {
        line( histImages[0], Point( bin_w*(i-1), HIST_HEIGHT - cvRound(b_hist.at<float>(i-1)) ),
              Point( bin_w*(i), HIST_HEIGHT - cvRound(b_hist.at<float>(i)) ),
              Scalar( 255, 0, 0), 2, 8, 0  );
        line( histImages[1], Point( bin_w*(i-1), HIST_HEIGHT - cvRound(g_hist.at<float>(i-1)) ),
              Point( bin_w*(i), HIST_HEIGHT - cvRound(g_hist.at<float>(i)) ),
              Scalar( 0, 255, 0), 2, 8, 0  );
        line( histImages[2], Point( bin_w*(i-1), HIST_HEIGHT - cvRound(r_hist.at<float>(i-1)) ),
              Point( bin_w*(i), HIST_HEIGHT - cvRound(r_hist.at<float>(i)) ),
              Scalar( 0, 0, 255), 2, 8, 0  );
    }
    imshow("Blue Histogram", histImages[0] );
    imshow("Green Histogram", histImages[1] );
    imshow("Red Histogram", histImages[2] );
}



void ImageAnalysis::GenerateRGBHist(const Mat &Image){
    vector<Mat> bgr_planes;
    split( Image, bgr_planes );
    int HIST_SIZE = 256;
    float range[] = { 0, 256 }; //the upper boundary is exclusive
    const float* histRange = { range };
    bool uniform = true, accumulate = false;
    calcHist( &bgr_planes[0], 1, 0, Mat(), b_hist, 1, &HIST_SIZE, &histRange, uniform, accumulate );
    calcHist( &bgr_planes[1], 1, 0, Mat(), g_hist, 1, &HIST_SIZE, &histRange, uniform, accumulate );
    calcHist( &bgr_planes[2], 1, 0, Mat(), r_hist, 1, &HIST_SIZE, &histRange, uniform, accumulate );
    normalize(b_hist, b_hist, 0, histImages[0].rows, NORM_MINMAX, -1, Mat() );
    normalize(g_hist, g_hist, 0, histImages[1].rows, NORM_MINMAX, -1, Mat() );
    normalize(r_hist, r_hist, 0, histImages[2].rows, NORM_MINMAX, -1, Mat() );
}

void ImageAnalysis::update(){
  // color model conversions
  cvtColor(*frame, hsvImage, CV_BGR2HSV);

  // frame manipulation & updates
  Mat hsvFilteredImage = hsvFilter();
  Mat bgrFilteredImage = bgrFilter();
  Mat binaryFilteredImage = binaryFilter();
  //update histograms
  plotHist();
  imshow("HSV Filtered", hsvFilteredImage);
  imshow("BGR Filtered", bgrFilteredImage);
  imshow("Binary Filtered", binaryFilteredImage);
  imshow(screenName, *frame);
  
}

Mat ImageAnalysis::hsvFilter(){
  Mat result = Mat::zeros((*frame).size(), (*frame).type()), mask;
  int hMin, hMax, sMin, sMax, vMin, vMax;

  hMin = max(HSV_color[0] - epsilon, 0);
  sMin = max(HSV_color[1] - epsilon, 0);
  vMin = max(HSV_color[2] - epsilon, 0);
  hMax = min(HSV_color[0] + epsilon, 255);
  sMax = min(HSV_color[1] + epsilon, 255);
  vMax = min(HSV_color[2] + epsilon, 255);

  // Updates mask values with the corresponding H,S,V limits
  inRange(hsvImage, Scalar(hMin, sMin, vMin), Scalar(hMax, sMax, vMax), mask);

  // Combines the mask with the original image, as result we get only the filtered colors 
  (*frame).copyTo(result, mask);

  return result;
}

Mat ImageAnalysis::bgrFilter(){
  Mat result = Mat::zeros((*frame).size(), (*frame).type()), mask;
  int bMin, bMax, gMin, gMax, rMin, rMax;

  bMin = max(BGR_color[0] - epsilon, 0);
  gMin = max(BGR_color[1] - epsilon, 0);
  rMin = max(BGR_color[2] - epsilon, 0);
  bMax = min(BGR_color[0] + epsilon, 255);
  gMax = min(BGR_color[1] + epsilon, 255);
  rMax = min(BGR_color[2] + epsilon, 255);

  // Updates mask values with the corresponding B,G,R limits
  inRange(*frame, Scalar(bMin, gMin, rMin), Scalar(bMax, gMax, rMax), mask);

  // Combines the mask with the original image, as result we get only the filtered colors 
  (*frame).copyTo(result, mask);

  return result;
}

Mat ImageAnalysis::binaryFilter(){
  Mat result = frame->clone();

  for(int i=0; i<result.rows; i++){
    for(int x=0; x<result.cols; x++){
      int color_average = (result.at<Vec3b>(i,x)[0] + result.at<Vec3b>(i,x)[1] + result.at<Vec3b>(i,x)[2])/3;
      if(color_average < 127){
        result.at<Vec3b>(i,x)[0] = 0;
        result.at<Vec3b>(i,x)[1] = 0;
        result.at<Vec3b>(i,x)[2] = 0;
      }else{
        result.at<Vec3b>(i,x)[0] = 255;
        result.at<Vec3b>(i,x)[1] = 255;
        result.at<Vec3b>(i,x)[2] = 255;
      }
    }
  }
  return result;
}

void ImageAnalysis::onMouse(int event, int x, int y, int, void* userdata){
  ImageAnalysis* imageAnalysis = reinterpret_cast<ImageAnalysis*>(userdata);
  imageAnalysis->onMouse(event, x, y);
}

void ImageAnalysis::onMouse(int event, int x, int y){
  switch (event){
    case CV_EVENT_LBUTTONDOWN:
        cout << "  Mouse X, Y: " << x << ", " << y << endl;
        BGR_color = (*frame).at<Vec3b>(Point(x,y));
        HSV_color = (hsvImage).at<Vec3b>(Point(x,y));
        cout << "B = " << (int)BGR_color[0] << endl;
        cout << "G = " << (int)BGR_color[1] << endl;
        cout << "R = " << (int)BGR_color[2] << endl;
        cout << "H = " << (int)HSV_color[0] << endl;
        cout << "S = " << (int)HSV_color[1] << endl;
        cout << "V = " << (int)HSV_color[2] << endl;
        break;
    case CV_EVENT_MOUSEMOVE: 
        break;
    case CV_EVENT_LBUTTONUP:
        break;
  }
}