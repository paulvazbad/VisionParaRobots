#include "ImageAnalysis.h"
#include <iostream>
#include <string.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#if WIN32
#include <windows.h>
#else
#include <X11/Xlib.h>
#endif
using namespace std;
using namespace cv;

ImageAnalysis::ImageAnalysis(Mat &image, string screenName)
{
  frame = &image;
  this->screenName = screenName;
  epsilon = 35;
  histImages[0] = Mat(HIST_HEIGHT, HIST_WIDTH + 50, CV_8UC3, Scalar(0, 0, 0));
  histImages[1] = Mat(HIST_HEIGHT, HIST_WIDTH + 50, CV_8UC3, Scalar(0, 0, 0));
  histImages[2] = Mat(HIST_HEIGHT, HIST_WIDTH + 50, CV_8UC3, Scalar(0, 0, 0));
  GenerateRGBHist(*frame);
  namedWindow(screenName);
  setMouseCallback(screenName, onMouse, this);
  cvtColor(*frame, hsvImage, CV_BGR2HSV);

  int width1, height1;
  height1 = frame->size().height;
  width1 = frame->size().width;
  int width2, height2;
  getScreenResolution(width2, height2);
  windowsVerticalPosition = height2 / 3;
  windowsHeightRatio = (double(windowsVerticalPosition) * 0.8) / height1;
  windowsSecondColumnPosition = windowsHeightRatio * width1 * 1.2;
}

void ImageAnalysis::getScreenResolution(int &width, int &height)
{
#if WIN32
  width = (int)GetSystemMetrics(SM_CXSCREEN);
  height = (int)GetSystemMetrics(SM_CYSCREEN) * 0.9;
#else
  Display *disp = XOpenDisplay(NULL);
  Screen *scrn = DefaultScreenOfDisplay(disp);
  width = scrn->width;
  height = scrn->height * 0.97;
  verticalOffset = scrn->height * 0.03;
#endif
}

void ImageAnalysis::plotLines()
{
  //Calculate line position in hist
  float position_x_blue = (bin_w * BGR_color[0]);
  float position_x_green = (bin_w * BGR_color[1]);
  float position_x_red = (bin_w * BGR_color[2]);
  //PLot line in hists
  //Copy of the hists
  Mat histImagesCopy[3];
  for (int i = 0; i < 3; i++)
  {
    //Add space for axis
    histImagesCopy[i] = histImages[i].clone();
    Mat row = Mat(50, HIST_WIDTH + 50, CV_8UC3, Scalar(0, 0, 0));
    histImagesCopy[i].push_back(row);
  }
  line(histImagesCopy[0], Point(position_x_blue, 0), Point(position_x_blue, HIST_HEIGHT),
       Scalar(255, 255, 255), 1, 8, 0);
  line(histImagesCopy[1], Point(position_x_green, 0), Point(position_x_green, HIST_HEIGHT),
       Scalar(255, 255, 255), 1, 8, 0);
  line(histImagesCopy[2], Point(position_x_red, 0), Point(position_x_red, HIST_HEIGHT),
       Scalar(255, 255, 255), 1, 8, 0);
  imshow("Blue Histogram", histImagesCopy[0]);
  imshow("Green Histogram", histImagesCopy[1]);
  imshow("Red Histogram", histImagesCopy[2]);
}

void ImageAnalysis::GenerateRGBHist(const Mat &Image)
{
  cout << "Generate RGBHist" << endl;
  vector<Mat> bgr_planes;
  split(Image, bgr_planes);
  int HIST_SIZE = 256;
  float range[] = {0, 256}; //the upper boundary is exclusive
  const float *histRange = {range};
  bool uniform = true, accumulate = false;
  calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &HIST_SIZE, &histRange, uniform, accumulate);
  calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &HIST_SIZE, &histRange, uniform, accumulate);
  calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &HIST_SIZE, &histRange, uniform, accumulate);
  normalize(b_hist, b_hist, 0, histImages[0].rows, NORM_MINMAX, -1, Mat());
  normalize(g_hist, g_hist, 0, histImages[1].rows, NORM_MINMAX, -1, Mat());
  normalize(r_hist, r_hist, 0, histImages[2].rows, NORM_MINMAX, -1, Mat());
  bin_w = cvRound((double)HIST_WIDTH / HIST_SIZE);
  //Plots main lines of the histogram
  const int INITIAL_OFFSET = 10;
  for (int i = 1; i < HIST_SIZE; i++)
  {
    line(histImages[0], Point(bin_w * (i - 1), HIST_HEIGHT - cvRound(b_hist.at<float>(i - 1))),
         Point(bin_w * (i), HIST_HEIGHT - cvRound(b_hist.at<float>(i))),
         Scalar(255, 0, 0), 2, 8, 0);
    line(histImages[1], Point(bin_w * (i - 1), HIST_HEIGHT - cvRound(g_hist.at<float>(i - 1))),
         Point(bin_w * (i), HIST_HEIGHT - cvRound(g_hist.at<float>(i))),
         Scalar(0, 255, 0), 2, 8, 0);
    line(histImages[2], Point(bin_w * (i - 1), HIST_HEIGHT - cvRound(r_hist.at<float>(i - 1))),
         Point(bin_w * (i), HIST_HEIGHT - cvRound(r_hist.at<float>(i))),
         Scalar(0, 0, 255), 2, 8, 0);
  }
}

void ImageAnalysis::update()
{
  // color model conversions
  cvtColor(*frame, hsvImage, CV_BGR2HSV);

  // frame manipulation & updates
  Mat hsvFilteredImage = hsvFilter();
  Mat bgrFilteredImage = bgrFilter();
  Mat binaryConvertedImage = binaryFilter();
  Mat bgrToHsvConvertedImage = bgrToHsv();
  Mat bgrToYiqConvertedImage = bgrToYIQ();
  //update histograms
  //plotLines();
  //namedWindow(screenName, CV_WINDOW_AUTOSIZE);
  //
  Mat outImg;

  moveWindow(screenName, 0, verticalOffset);
  resize(*frame, outImg, cv::Size(), windowsHeightRatio, windowsHeightRatio);
  imshow(screenName, outImg);

  moveWindow("HSV Filtered", windowsSecondColumnPosition, verticalOffset);
  resize(hsvFilteredImage, outImg, cv::Size(), windowsHeightRatio, windowsHeightRatio);
  imshow("HSV Filtered", outImg);

  moveWindow("BGR Filtered", 0, windowsVerticalPosition + verticalOffset);
  resize(bgrFilteredImage, outImg, cv::Size(), windowsHeightRatio, windowsHeightRatio);
  imshow("BGR Filtered", outImg);

  moveWindow("Binary Converted", windowsSecondColumnPosition, windowsVerticalPosition + verticalOffset);
  resize(binaryConvertedImage, outImg, cv::Size(), windowsHeightRatio, windowsHeightRatio);
  imshow("Binary Converted", outImg);

  moveWindow("HSV Converted", 0, windowsVerticalPosition * 2 + verticalOffset);
  resize(bgrToHsvConvertedImage, outImg, cv::Size(), windowsHeightRatio, windowsHeightRatio);
  imshow("HSV Converted", outImg);

  moveWindow("YIQ Converted", windowsSecondColumnPosition, windowsVerticalPosition * 2 + verticalOffset);
  resize(bgrToYiqConvertedImage, outImg, cv::Size(), windowsHeightRatio, windowsHeightRatio);
  imshow("YIQ Converted", outImg);
}

Mat ImageAnalysis::hsvFilter()
{
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

Mat ImageAnalysis::bgrFilter()
{
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

Mat ImageAnalysis::binaryFilter()
{
  Mat result = frame->clone();
  int luminosity_average = 0;

  for (int i = 0; i < result.rows; i++)
  {
    for (int x = 0; x < result.cols; x++)
    {
      double b = result.at<Vec3b>(i, x)[0];
      double g = result.at<Vec3b>(i, x)[1];
      double r = result.at<Vec3b>(i, x)[2];
      luminosity_average += int(0.299 * r + 0.587 * g + 0.114 * b);
    }
  }
  luminosity_average /= (result.cols * result.rows);

  for (int i = 0; i < result.rows; i++)
  {
    for (int x = 0; x < result.cols; x++)
    {
      int color_average = (result.at<Vec3b>(i, x)[0] + result.at<Vec3b>(i, x)[1] + result.at<Vec3b>(i, x)[2]) / 3;
      if (color_average < luminosity_average)
      {
        result.at<Vec3b>(i, x)[0] = 0;
        result.at<Vec3b>(i, x)[1] = 0;
        result.at<Vec3b>(i, x)[2] = 0;
      }
      else
      {
        result.at<Vec3b>(i, x)[0] = 255;
        result.at<Vec3b>(i, x)[1] = 255;
        result.at<Vec3b>(i, x)[2] = 255;
      }
    }
  }
  return result;
}

//Formulas from: https://www.eembc.org/techlit/datasheets/yiq_consumer.pdf
Mat ImageAnalysis::bgrToYIQ()
{
  Mat result = frame->clone();

  for (int z = 0; z < result.rows; z++)
  {
    for (int x = 0; x < result.cols; x++)
    {
      double b = result.at<Vec3b>(z, x)[0];
      double g = result.at<Vec3b>(z, x)[1];
      double r = result.at<Vec3b>(z, x)[2];
      int y = int(0.299 * r + 0.587 * g + 0.114 * b);
      int i = int(0.596 * r - 0.275 * g - 0.321 * b) + 127;
      int q = int(0.212 * r - 0.523 * g + 0.311 * b) + 127;

      //Substitute on resulting image
      result.at<Vec3b>(z, x)[0] = q;
      result.at<Vec3b>(z, x)[1] = i;
      result.at<Vec3b>(z, x)[2] = y;
    }
  }
  return result;
}

Mat ImageAnalysis::bgrToHsv()
{
  Mat result = frame->clone();
  cvtColor(*frame, result, CV_BGR2HSV);
  return result;
}

void ImageAnalysis::onMouse(int event, int x, int y, int, void *userdata)
{
  ImageAnalysis *imageAnalysis = reinterpret_cast<ImageAnalysis *>(userdata);
  imageAnalysis->onMouse(event, x, y);
}

void ImageAnalysis::onMouse(int event, int x, int y)
{
  switch (event)
  {
  case CV_EVENT_LBUTTONDOWN:
    cout << "  Mouse X, Y: " << x << ", " << y << endl;
    BGR_color = (*frame).at<Vec3b>(Point(x, y));
    HSV_color = (hsvImage).at<Vec3b>(Point(x, y));
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