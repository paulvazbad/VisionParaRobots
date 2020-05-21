#include <iostream>
#include <string.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "ObjectAnalysis.h"
#include <bits/stdc++.h> 
using namespace std;
using namespace cv;

// Validates input. True if static media / False if not
bool inputValidation(int argc, char **argv, Mat &image, VideoCapture &cap)
{
  string path;

  if (argc > 1)
  {
    path = string(argv[1]);
    if (int(path.find(".jpg")) > -1)
    {
      image = imread(path, CV_LOAD_IMAGE_COLOR);
      return true;
    }
  }
  
  if(cap.open("rtsp://192.168.0.8:8080/h264_pcm.sdp")) {
    if(cap.read(image)) {
      return false;
    }
  }

  argc > 1 && string(argv[1]).find(".mp4") ? cap.open(path) : cap.open(0);
  cap >> image;
  return false;
}

int main(int argc, char *argv[])
{ 
  Mat image;
  VideoCapture cap;
  ios_base::sync_with_stdio(false); 
  cin.tie(NULL);  
  srand(time(0)); 
  bool isStatic = inputValidation(argc, argv, image, cap);
  ObjectAnalysis objectAnalysis = ObjectAnalysis(image, "Original Image");

  string mode = "default";
  if(argc > 2 && (string(argv[2]) == "calibrate" || string(argv[2]) == "train" || string(argv[2]) == "capture"))
    mode = string(argv[2]);
  
  cout << "Running on " << mode << " mode!\n\n";

  if(mode == "train")
  {
    cout<< "Training really hard!" << endl;
    objectAnalysis.trainDataset();
  }
  else if(mode == "calibrate")
  {
    for(;;)
    {
      if(!isStatic)
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
  else if(mode == "capture")
  {
    for(;;)
    {
      if(!isStatic)
        cap >> image;

      imshow("Current frame",image);

      int x = waitKey(30);
      if (x == 's')
      {
          objectAnalysis.captureTrainData(image);
      }
      else if (x == 'x')
      {
          break;
      }
    }
  }
  // else
  //   objectAnalysis.findRegions(2,1000,2000);
  
  return 0;
}