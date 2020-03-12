/*
 * Title: Flipped Image Sample (Cascaron)
 * Class: Vision para Robot
 * Instructor: Dr. Jose Luis Gordillo (http://LabRob.mty.itesm.mx/)
 * Code: Manlio Barajas (manlito@gmail.com), Alberto Jahuey (A01039835@itesm.mx)
 * Institution: Tec de Monterrey, Campus Monterrey
 * Date: January 10, 2012 Last Update: January 30, 2019
 *
 * Description: This program takes input from a camera (recognizable by
 * OpenCV) and it flips it horizontally.
 * "Basic" version uses frequently the "Cv::Mat::At" method which slows down
 * performance. This program has illustrative purposes, provided the existence
 * of cv::flip method.
 *
 *
 * This programs uses OpenCV http://opencv.willowgarage.com/wiki/
 */

#include <iostream>
#include <string.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace std;
using namespace cv;

void showCamera(){
  VideoCapture camera = VideoCapture(0);
  bool isCameraAvailable = camera.isOpened();

  /* Create images where captured and transformed frames are going to be stored
   */
  Mat currentImage;
  Mat flippedImage;
  /* Clean the terminal */
  cout << "\033[2J\033[1;1H";
  cout << "Basic Show Image \t|\tUse 'x' or 'Esc' to terminate execution\n";
  while (true) {
    /* Obtain a new frame from camera */
    if (isCameraAvailable) {
      camera.read(currentImage);
    } else {
      currentImage = imread("PlaceholderImage.jpg", CV_LOAD_IMAGE_COLOR);
    }

    if (currentImage.size().width <= 0 && currentImage.size().height <= 0) {
      cout << "ERROR: Camera returned blank image, check connection\n";
      break;
    }
    /* If 'x' is pressed, exit program */
    char key = waitKey(1);
    if(key == 'x' || key == 27 ){ // 27 = ESC
      break;
    }
    imshow("Imagen",currentImage);
}
}
void initializeMat(double RGBValues[3][256]){
  for(int color_value = 0; color_value<256; color_value++){
    for(int color=0; color<3; color++){
      RGBValues[color][color_value]=0;
    }
  }
  
}

void plotHist(double ColorValues[256], string canvasName, const int size_Y, const int size_X){

  Mat canvas(256, 1000, CV_8UC3, cv::Scalar(0,0,0));
  //White Background
  const int Pos_X = 5; 
  //Draw Y Axis (Size of Y == Max value in histograms)
  imshow(canvasName,canvas);
  
  cout<<"Opened canvas"<<endl;
  
    
}

void GenerateRGBHist(const Mat &Image,double RGBValues[3][256]){
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
  string canvasNames[3]={"Blue","Green","Red"};

  for(int i=0; i<3; ++i){
    cout<<"IMSHOW"<<endl;
    plotHist(RGBValues[i],canvasNames[i],Image.rows,Image.cols);
  }
  waitKey(0);
}

void printValue(double ColorValues[256]){
  for(int i=0; i<256; i++){
    cout<<ColorValues[i]<<endl;
  }
}
int main(int argc, char *argv[]) {
  /* First, open camera device */
    Mat currentImage = imread("PlaceholderImage.jpg", CV_LOAD_IMAGE_COLOR);
    imshow("Window",currentImage);
    waitKey(6);
    double RGBValues[3][256];
    GenerateRGBHist(currentImage,RGBValues);
    
    
  }