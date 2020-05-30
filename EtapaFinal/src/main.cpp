#include <iostream>
#include <string.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
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
  return 0;
}