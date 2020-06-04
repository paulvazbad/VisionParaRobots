#include <iostream>
#include <string.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <bits/stdc++.h>
#include <regex> 
#include "FindParkingSpace.h"
using namespace std;
using namespace cv;

const short FIGURE_DETECTION = 0;
const short FIGURE_CALIBRATION = 1;
const short PARKING_SPOT_DEMO = 2;
const short EXIT = 3;
const short CAPTURE_DATA = 4;
int image_counter = 0;
unordered_map<string, int> menu_values = {
    {"c", FIGURE_CALIBRATION},
    {"p", PARKING_SPOT_DEMO},
    {"x", EXIT},
    {"k", CAPTURE_DATA}};

// Validates input. True if static media / False if not
bool inputValidation(int argc, char **argv, Mat &image, VideoCapture &cap)
{
  string path;

  if (argc > 1)
  {
    path = string(argv[1]);
    if (int(path.find(".jpg")) > -1)
    {
      cout << "Image read" << endl;
      image = imread(path, CV_LOAD_IMAGE_COLOR);
      return true;
    }
    else if(path == "saved")
    {
      cout<<"Opening..."<<"./egypt/"+ to_string(image_counter)+".jpg"<<endl;
      image = imread("./egypt/"+ to_string(image_counter)+".jpg", CV_LOAD_IMAGE_COLOR);
      image_counter = (image_counter+1)%9;
      return true;
    }
  }

  if (cap.open("rtsp://192.168.0.8:8080/h264_pcm.sdp"))
  {
    if (cap.read(image))
    {
      return false;
    }
  }

  argc > 1 && string(argv[1]).find(".mp4") ? cap.open(path) : cap.open(0);
  cap >> image;
  return false;
}

void figureDetection(ObjectAnalysis &objectAnalysis, bool isStatic, VideoCapture cap, Mat figure_image)
{
    if (!isStatic)
      cap >> figure_image;

    //objectAnalysis.prepareResults(figure_image);
    objectAnalysis.finalizeFiltering();
    waitKey(0);
}

int main(int argc, char *argv[])
{
  Mat image;
  VideoCapture cap;
  bool isStatic = inputValidation(argc, argv, image, cap);
  ObjectAnalysis objectAnalysis = ObjectAnalysis(image, "Figures Image");
  Mat parking_image_clean = imread("./estacionamiento_clean.jpg", CV_LOAD_IMAGE_COLOR);
  string mode = "p";
  while (menu_values.at(mode) != EXIT)
  {
    cout << "Que modo quieres correr?" << endl;
    cout << "c: calibracion de figuras" << endl;
    cout << "p: resultados de parking spot" << endl;
    cout << "x: exit" << endl;
    cin >> mode;
    switch (menu_values.at(mode))
    {

    case FIGURE_CALIBRATION:
    {
      objectAnalysis.initCalibration();
      for (;;)
      {
        if (!isStatic)
          cap >> image;
        objectAnalysis.filterImage(image);
        int x = waitKey(30);
        if (x == 'x')
        {
          objectAnalysis.save_calibration_values();
          destroyAllWindows();
          break;
        }
      }
    }
    break;
    case PARKING_SPOT_DEMO:
    {
      bool path = false;
      int entrance = -1;
      objectAnalysis.getObjectAnalysisResults(image, path, entrance);
      if(entrance == -1)
      {
        cout<<"Entrance not found, please calibrate!"<<endl;
        break;
      }
      FindParkingSpace findParkingSpace = FindParkingSpace(parking_image_clean, "Parking Lot Image");
      for (;;)
      {
        if (!isStatic)
        {
          cap >> image;
          objectAnalysis.getObjectAnalysisResults(image, path, entrance);
        }
        findParkingSpace.findPath(entrance, path);
        int x = waitKey(30);
        if (x == 'x')
        {
          destroyAllWindows();
          break;
        }
        if( x == 'p')
        {
          findParkingSpace.showRobotTravel();
        }
      }
      bool isStatic = inputValidation(argc, argv, image, cap);
    }
    break;
    case CAPTURE_DATA:
    {
      cout<<"Running capture data mode"<<endl;
      for (;;)
      {
        if (!isStatic)
          cap >> image;
        imshow("Image captured",image);
        int x = waitKey(30);
        if (x == 'x')
        {
          break;
        }else if(x == 'k'){
          objectAnalysis.captureTrainData(image);
        }
      }
    }
    case EXIT:
    {
      break;
    }
    default:
    {
      mode = "x";
      break;
    }
    }
  }

  return 0;
}