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
#include "ImageAnalysis.h"
using namespace std;
using namespace cv;

// Validates input. True if static media / False if not
bool inputValidation(int &argc, char** &argv, Mat &image, VideoCapture &cap){
  string path;
  
  if(argc > 1){
    path = string(argv[1]);
    if(int(path.find(".jpg")) > -1){
      image = imread(path, CV_LOAD_IMAGE_COLOR);
      return true;
    }
  }

  argc > 1 && string(argv[1]).find(".mp4") ? cap.open(path) : cap.open(0);
  cap >> image;
  return false;
}

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

void printValue(double ColorValues[256]){
  for(int i=0; i<256; i++){
    cout<<ColorValues[i]<<endl;
  }
}

int main(int argc, char *argv[]){
  Mat image;
  VideoCapture cap;
  
  bool isStatic = inputValidation(argc, argv, image, cap);

  if(!image.data){
    cout <<  "Could not open or find the image/video\n";
    return -1;
  }

  ImageAnalysis imageAnalysis = ImageAnalysis(image, "Image");

  while(true){
    if(!isStatic)
      cap >> image;
    
    if(image.empty()){
        cout << "No image has been found\n";
        break;
    }

    imageAnalysis.update();

    if(waitKey(30) == 27)
      break;

  }
  
  return 0;
}