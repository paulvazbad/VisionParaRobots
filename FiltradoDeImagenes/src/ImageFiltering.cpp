#include "ImageFiltering.h"
#include <iostream>
#include <string.h>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <math.h> /* exp */
#if WIN32
#include <windows.h>
#else
#include <X11/Xlib.h>
#endif
using namespace std;
using namespace cv;

ImageFiltering::ImageFiltering(Mat &image, string screenName)
{
  frame = &image;
  this->screenName = screenName;
  namedWindow(screenName);
  namedWindow("Gray Converted");
  setMouseCallback("Gray Converted", onMouse, this);
  int width1, height1;
  height1 = frame->size().height;
  width1 = frame->size().width;
  int width2, height2;
  getScreenResolution(width2, height2);
  windowsVerticalPosition = height2 / 3;
  windowsHeightRatio = (double(windowsVerticalPosition) * 0.8) / height1;
  this->screenName = screenName;
  imshow(screenName, image);
  grayscaleImage = bgrToGray();
  gaussianFilter();
  laplaceFilter();
  logFilter();
  edgeDetectionFilter();
  enhancementFilter();
}
void ImageFiltering::printImageInfo(int x, int y)
{
  int KS = 5;
  cout << "Rows: " << grayscaleImage.rows << endl;
  cout << "Cols " << grayscaleImage.cols << endl;
  cout << "Channels: " << grayscaleImage.channels() << endl;

  for (int j = y - (KS / 2); j <= y + (KS / 2); ++j)
  {
    for (int i = x - (KS / 2); i <= x + (KS / 2); ++i)
    {
      if (i > 0 && i < grayscaleImage.cols && j > 0 && j < grayscaleImage.rows)
      {
        double value = grayscaleImage.at<Vec3b>(i, j)[0];

        cout << value << " ";
      }
      else
      {
        cout << "NaN ";
      }
    }
    cout << endl;
  }
}

void ImageFiltering::getScreenResolution(int &width, int &height)
{
#if WIN32
  width = (int)GetSystemMetrics(SM_CXSCREEN);
  height = (int)GetSystemMetrics(SM_CYSCREEN) * 0.9;
#else
  Display *disp = XOpenDisplay(NULL);
  Screen *scrn = DefaultScreenOfDisplay(disp);
  width = scrn->width * 0.95;
  height = scrn->height * 0.97;
  windowFullVerticalSize = scrn->height * 0.80;
  windowFullHorizontalSize = scrn->width * 0.88;
  verticalOffset = scrn->height * 0.03;
  horizontalOffset = scrn->width * 0.05;
#endif
}

void ImageFiltering::update()
{
  Mat outImageHelper;
  resize(grayscaleImage, outImageHelper, cv::Size(), windowsHeightRatio, windowsHeightRatio);
  imshow("Gray Converted", grayscaleImage);
}

Mat ImageFiltering::binaryFilter()
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

Mat ImageFiltering::bgrToGray()
{
  Mat result = frame->clone();
  cvtColor(*frame, result, CV_BGR2GRAY);
  return result;
}

void ImageFiltering::onMouse(int event, int x, int y, int, void *userdata)
{
  ImageFiltering *imageFiltering = reinterpret_cast<ImageFiltering *>(userdata);
  imageFiltering->onMouse(event, x, y);
}

void ImageFiltering::onMouse(int event, int x, int y)
{
  switch (event)
  {
  case CV_EVENT_LBUTTONDOWN:
    cout << "  Mouse X, Y: " << x << ", " << y << endl;
    printImageInfo(x, y);
    break;
  case CV_EVENT_MOUSEMOVE:
    break;
  case CV_EVENT_LBUTTONUP:
    break;
  }
}

//Median Filter
void ImageFiltering::medianFilter()
{
  Mat copy = grayscaleImage.clone();
  medianBlur(copy, copy, 3);
  imshow("median blur", copy);
}

void ImageFiltering::averageFilter()
{
  Mat copy = grayscaleImage.clone();
  Mat kernel = (Mat_<double>(5, 5) << 0.04, 0.04, 0.04, 0.04, 0.04,
                0.04, 0.04, 0.04, 0.04, 0.04,
                0.04, 0.04, 0.04, 0.04, 0.04,
                0.04, 0.04, 0.04, 0.04, 0.04,
                0.04, 0.04, 0.04, 0.04, 0.04);
  cout << kernel << endl;
  filter2D(copy, copy, -1, kernel, Point(-1, -1), 0, BORDER_DEFAULT);
  imshow("average blur", copy);
}

void ImageFiltering::gaussianFilter()
{
  Mat copy = grayscaleImage.clone();
  cout << getGaussianKernel(5, 1.41421356237, CV_32F) << endl;
  //generate gaussianFilter
  Mat kernel(5, 5, cv::DataType<double>::type);
  double sigma_squared = 1;
  int posx = -2;
  for (int col = 0; col < 5; col++)
  {
    int posy = -2;
    for (int row = 0; row < 5; row++)
    {
      double up = -(pow(posx, 2) + pow(posy, 2)) / (2 * sigma_squared);
      double gauss_value = 5 * exp(up);
      cout << double(round(gauss_value)) << " at: " << row << col << endl;
      kernel.at<double>(row, col) = double(round(gauss_value)) / 25;
      posy++;
    }
    posx++;
  }
  cout << kernel << endl;
  filter2D(copy, copy, -1, kernel, Point(-1, -1), 0, BORDER_DEFAULT);
  //Size seven(7,7);
  //GaussianBlur(copy,copy,seven,1.41421356237,1.41421356237,BORDER_DEFAULT);
  imshow("gaussian blur", copy);
}

void ImageFiltering::laplaceFilter(){
  Mat src, dst;
  int kernel_size = 3;
  int scale = 1;
  int delta = 0;
  int ddepth = CV_16S;
  src = grayscaleImage.clone();

  /// Remove noise by blurring with a Gaussian filter
  GaussianBlur( src, src, Size(3,3), 0, 0, BORDER_DEFAULT );
  /// Create window
  namedWindow( "Laplace filter", CV_WINDOW_AUTOSIZE );
  /// Apply Laplace function
  Mat abs_dst;
  Laplacian( src, dst, ddepth, kernel_size, scale, delta, BORDER_DEFAULT );
  convertScaleAbs( dst, abs_dst ); //converts to CV_8U
  imshow( "Laplace filter", abs_dst );
}

void ImageFiltering::logFilter(){
  Mat src, dst;
  Mat kernel = (Mat_<double>(3,3) << 0,1,0,1,-4,1,0,1,0);
  Point anchor = Point( -1, -1 );
  double delta = 0;
  int ddepth = CV_16S;
  src = grayscaleImage.clone();

  /// Remove noise by blurring with a Gaussian filter
  GaussianBlur( src, src, Size(3,3), 0, 0, BORDER_DEFAULT );
  /// Create window
  namedWindow( "LoG Filter", CV_WINDOW_AUTOSIZE );
  /// Apply filter
  Mat abs_dst;
  filter2D(src, dst, ddepth , kernel, anchor, delta, BORDER_DEFAULT );
  convertScaleAbs( dst, abs_dst ); //converts to CV_8U
  imshow( "LoG Filter", abs_dst);
}

void ImageFiltering::edgeDetectionFilter(){
  Mat src, edges, dst;
  int ratio = 3;
  int kernel_size = 3;
  int lowThreshold = 100;
  src = grayscaleImage.clone();

  /// Remove noise by blurring with a Gaussian filter
  GaussianBlur( src, src, Size(3,3), 0, 0, BORDER_DEFAULT );
  /// Create window
  namedWindow( "Canny Edge Detection Filter", CV_WINDOW_AUTOSIZE );
  /// Apply filter
  Canny( src, edges, lowThreshold, lowThreshold*ratio, kernel_size );
  dst = Scalar::all(0);
  src.copyTo( dst, edges);
  imshow( "Canny Edge Detection Filter", dst);
}

void ImageFiltering::enhancementFilter(){
  Mat src, dst;
  Mat kernel = (Mat_<double>(3,3) << -1, -1, -1, -1, 9, -1, -1, -1, -1);
  Point anchor = Point( -1, -1 );
  double delta = 0;
  int ddepth = CV_16S;
  src = grayscaleImage.clone();

  /// Remove noise by blurring with a Gaussian filter
  GaussianBlur( src, src, Size(3,3), 0, 0, BORDER_DEFAULT );
  /// Create window
  namedWindow( "Enhancement Filter", CV_WINDOW_AUTOSIZE );
  /// Apply filter
  Mat abs_dst;
  filter2D(src, dst, ddepth , kernel, anchor, delta, BORDER_DEFAULT );
  convertScaleAbs( dst, abs_dst ); //converts to CV_8U
  imshow( "Enhancement Filter", abs_dst);
}

void ImageFiltering::endProgram()
{
  destroyAllWindows();
}