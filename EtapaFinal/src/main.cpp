#include <iostream>
#include <string.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <bits/stdc++.h>
#include "FindParkingSpace.h"
using namespace std;
using namespace cv;

const short FIGURE_DETECTION = 0;
const short FIGURE_CALIBRATION = 1;
const short PARKING_SPOT_DEMO = 2;
const short EXIT = 3;
unordered_map<string, int> menu_values = {
                                          {"c", FIGURE_CALIBRATION},
                                          {"p", PARKING_SPOT_DEMO},
                                          {"x", EXIT}};

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

void figureDetection(ObjectAnalysis &objectAnalysis, bool isStatic, VideoCapture cap, Mat figure_image){
  bool analyzeOrCalibrate = false;
    for(;;)
    {
      if(!isStatic)
        cap >> figure_image;

      if(!analyzeOrCalibrate){
        objectAnalysis.filterImage(figure_image);
      }else if(!isStatic){
        objectAnalysis.prepareResults(figure_image);
      }

      int x = waitKey(30);
      if (x == 'r')
      {
        objectAnalysis.prepareResults(figure_image);
        objectAnalysis.save_calibration_values();
        objectAnalysis.finalizeFiltering();
        analyzeOrCalibrate = true;
      }
      else if (x == 'c')
      {
        objectAnalysis.closeResults();
        analyzeOrCalibrate = false;
        objectAnalysis.filterImage(figure_image);
      }
      else if (x == 'x')
      {
          break;
      }
    }
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
      for (;;)
      {
        if (!isStatic)
          cap >> image;
        objectAnalysis.filterImage(image);
        int x = waitKey(30);
        if (x == 's')
        {
          break;
        }
      }
      objectAnalysis.save_calibration_values();
    }
    break;
    case PARKING_SPOT_DEMO:
    {
      figureDetection(objectAnalysis,isStatic,cap,image);
      FindParkingSpace findParkingSpace = FindParkingSpace(parking_image_clean, "Original Image");
    }
    break;
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