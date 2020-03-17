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
  namedWindow(screenName);
  setMouseCallback(screenName, onMouse, this);
}

void ImageAnalysis::plotHist(double ColorValues[256], string histogramName){
  Mat canvas(256, 1000, CV_8UC3, cv::Scalar(0,0,0));
  //White Background
  const int Pos_X = 5; 
  //Draw Y Axis (Size of Y == Max value in histograms)
  imshow(histogramName,canvas);
  cout<<"Opened canvas"<<endl;
  
    
}

void ImageAnalysis::initializeMat(double RGBValues[3][256]){
  for(int color_value = 0; color_value<256; color_value++){
    for(int color=0; color<3; color++){
      RGBValues[color][color_value]=0;
    }
  }
  
}

void ImageAnalysis::GenerateRGBHist(const Mat &Image,double RGBValues[3][256]){
  initializeMat(RGBValues);
  for(int row=0; row < Image.rows; ++row){
    for(int col=0; col < Image.cols; ++col){
      for (int i = 0; i < Image.channels(); ++i)
			{
        int color_value = Image.at<Vec3b>(row, col)[i];
				RGBValues[i][color_value]++;
			}
    }
  }
  string histogramNames[3]={"Blue","Green","Red"};

  for(int i=0; i<3; ++i){
    cout<<"IMSHOW"<<endl;
    plotHist(RGBValues[i],histogramNames[i]);
  }
  waitKey(0);
}

void ImageAnalysis::update(){
  // frame manipulation & updates

  imshow(screenName, *frame);
}

void ImageAnalysis::onMouse(int event, int x, int y, int, void* userdata){
  ImageAnalysis* imageAnalysis = reinterpret_cast<ImageAnalysis*>(userdata);
  imageAnalysis->onMouse(event, x, y);
}

void ImageAnalysis::onMouse(int event, int x, int y){
  Vec3b BGR_color, HSV_color;
  Mat fullImageHSV;
  cvtColor(*frame, fullImageHSV, CV_BGR2HSV);
  imshow("HSV", fullImageHSV);

  switch (event){
    case CV_EVENT_LBUTTONDOWN:
        cout << "  Mouse X, Y: " << x << ", " << y << endl;
        BGR_color = (*frame).at<Vec3b>(Point(x,y));
        HSV_color = (fullImageHSV).at<Vec3b>(Point(x,y));
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