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
  int const max_elem = 2;
  logFilter();
  edgeDetectionFilter();
  enhancementFilter();
<<<<<<< HEAD
  degradadoFilter();
  averageFilter();
  medianFilter();
  Mat binaryImage = binaryFilter();
  namedWindow( "Binary Filtered", CV_WINDOW_AUTOSIZE );
  resize(binaryImage, binaryImage, cv::Size(), 0.7, 0.7);
  imshow("Binary Filtered", binaryImage);
  sobelFilter();
  scharrFilter();
  erotion();
  dilation();
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
  resize(grayscaleImage, grayscaleImage, cv::Size(), 0.7, 0.7);
  imshow("Gray Converted", grayscaleImage);
}

Mat ImageFiltering::binaryFilter(Mat result)
{
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

  Mat erosion_dst, dilation_dst;
  int erosion_size = 1;
  int dilation_size = 1;
  int const max_kernel_size = 21;

  /// Create windows
  namedWindow( "Erosion", CV_WINDOW_AUTOSIZE );
  namedWindow( "Dilatacion", CV_WINDOW_AUTOSIZE );

  Mat element = getStructuringElement( MORPH_RECT,
                                       Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                       Point( erosion_size, erosion_size ) );

  /// Apply the erosion operation
  erode( result, erosion_dst, element );
  resize(erosion_dst, erosion_dst, cv::Size(), 0.7, 0.7);
  imshow( "Erosion", erosion_dst );

  Mat element2 = getStructuringElement( MORPH_RECT,
                                       Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                       Point( dilation_size, dilation_size ) );
  /// Apply the dilation operation
  dilate( result, dilation_dst, element2 );
  resize(dilation_dst, dilation_dst, cv::Size(), 0.7, 0.7);
  imshow( "Dilatacion", dilation_dst );

   /// Create windows
  namedWindow( "Open & Closing", CV_WINDOW_AUTOSIZE );
  erode( result, erosion_dst, element );
  dilate( erosion_dst, dilation_dst, element );
  resize(dilation_dst, dilation_dst, cv::Size(), 0.7, 0.7);
  imshow( "Open & Closing", dilation_dst );

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
  resize(copy, copy, cv::Size(), 0.7, 0.7);
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
  resize(copy, copy, cv::Size(), 0.7, 0.7);
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
  resize(copy, copy, cv::Size(), 0.7, 0.7);
  imshow("gaussian blur", copy);
}

void ImageFiltering::laplaceFilter(){
  Mat src, dst;
  int kernel_size = 3;
  int scale = 1;
  int delta = 0;
  int ddepth = CV_16S;
  src = grayscaleImage.clone();
  /// Create window
  namedWindow( "Laplace filter", CV_WINDOW_AUTOSIZE );
  /// Apply Laplace function
  Mat abs_dst;
  Laplacian( src, dst, ddepth, kernel_size, scale, delta, BORDER_DEFAULT );
  convertScaleAbs( dst, abs_dst ); //converts to CV_8U
  resize(abs_dst, abs_dst, cv::Size(), 0.7, 0.7);
  //normalize(abs_dst,  abs_dst, 0, 255, NORM_MINMAX);
  imshow( "Laplace filter", abs_dst );
}

void ImageFiltering::logFilter(){
  Mat src, dst, dst2;
  Mat kernel = (Mat_<double>(3,3) << 0,1,0,1,-4,1,0,1,0);
  Mat kernel2 = (Mat_<double>(5,5) << 0,0,-1,0,0,0,-1,-2,-1,0,-1,-2,16,-2,-1,0,-1,-2,-1,0,0,0,-1,0,0);
  Point anchor = Point( -1, -1 );
  double delta = 0;
  int ddepth = CV_16S;
  src = grayscaleImage.clone();

  /// Remove noise by blurring with a Gaussian filter
  GaussianBlur( src, src, Size(3,3), 0, 0, BORDER_DEFAULT );
  /// Create window
  namedWindow( "LoG Filter", CV_WINDOW_AUTOSIZE );
  namedWindow( "LoG Filter 5x5", CV_WINDOW_AUTOSIZE );
  /// Apply filter
  Mat abs_dst;
  Mat abs_dst2;
  filter2D(src, dst, ddepth , kernel, anchor, delta, BORDER_DEFAULT );
  convertScaleAbs( dst, abs_dst ); //converts to CV_8U
  resize(abs_dst, abs_dst, cv::Size(), 0.7, 0.7);
  imshow( "LoG Filter", abs_dst);
  filter2D(src, dst2, ddepth , kernel, anchor, delta, BORDER_DEFAULT );
  convertScaleAbs( dst, abs_dst2 ); //converts to CV_8U
  imshow( "LoG Filter", abs_dst2);
  imshow( "LoG Filter 5x5", abs_dst);
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
  resize(dst, dst, cv::Size(), 0.7, 0.7);
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
  resize(abs_dst, abs_dst, cv::Size(), 0.7, 0.7);
  imshow( "Enhancement Filter", abs_dst);
}

void ImageFiltering::degradadoFilter(){
  Mat src, dst;
  Mat kernel = (Mat_<double>(3,3) << -1, -2, -1, 0, 0, 0, 1, 2, 1);
  Point anchor = Point( -1, -1 );
  double delta = 0;
  int ddepth = CV_16S;
  src = grayscaleImage.clone();
  /// Create window
  namedWindow( "Degradado y", CV_WINDOW_AUTOSIZE );
  /// Apply filter
  Mat abs_dst, abs_dst2;
  filter2D(src, dst, ddepth , kernel, anchor, delta, BORDER_DEFAULT );
  convertScaleAbs( dst, abs_dst ); //converts to CV_8U
  resize(abs_dst, abs_dst, cv::Size(), 0.7, 0.7);
  imshow( "Degradado y", abs_dst);

  kernel = (Mat_<double>(3,3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
  filter2D(src, dst, ddepth , kernel, anchor, delta, BORDER_DEFAULT );
  convertScaleAbs( dst, abs_dst2 ); //converts to CV_8U
  namedWindow( "Degradado x", CV_WINDOW_AUTOSIZE );
  resize(abs_dst2, abs_dst2, cv::Size(), 0.7, 0.7);
  imshow( "Degradado x", abs_dst2);

  for (int col = 0; col < abs_dst.cols; col++)
  {
    for (int row = 0; row < abs_dst.rows; row++)
    {
      int maxval = max(abs_dst2.at<uchar>(col,row), abs_dst.at<uchar>(col,row));
      abs_dst2.at<uchar>(col,row) = maxval;
    }
  }
  namedWindow( "Degradado", CV_WINDOW_AUTOSIZE );
  imshow( "Degradado", abs_dst2);
}

void ImageFiltering::sobelFilter(){
  Mat src_gray = grayscaleImage.clone();
  Mat grad_x, grad_y, grad;
  Mat abs_grad_x, abs_grad_y;
  int scale = 1;
  int delta = 0;
  int ddepth = CV_16S;
  
  // Remove noise by blurring with a Gaussian filter
  GaussianBlur(src_gray, src_gray, Size(3,3), 0, 0, BORDER_DEFAULT);
  
  // Gradient X
  Sobel(src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
  convertScaleAbs(grad_x, abs_grad_x);

  // Gradient Y
  Sobel(src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
  convertScaleAbs(grad_y, abs_grad_y);

  namedWindow("Sobel Filter (dx)", CV_WINDOW_AUTOSIZE);
  namedWindow("Sobel Filter (dy)", CV_WINDOW_AUTOSIZE);
  imshow("Sobel Filter (dx)", abs_grad_x);
  imshow("Sobel Filter (dy)", abs_grad_y);
}

void ImageFiltering::scharrFilter(){
  Mat src_gray = grayscaleImage.clone();
  Mat grad_x, grad_y, grad;
  Mat abs_grad_x, abs_grad_y;
  int scale = 1;
  int delta = 0;
  int ddepth = CV_16S;
  
  // Remove noise by blurring with a Gaussian filter
  GaussianBlur(src_gray, src_gray, Size(3,3), 0, 0, BORDER_DEFAULT);

  // Gradient X
  Scharr(src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT);
  convertScaleAbs(grad_x, abs_grad_x);

  // Gradient Y
  Scharr(src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT);
  convertScaleAbs(grad_y, abs_grad_y);

  namedWindow("Scharr Filter (dx)", CV_WINDOW_AUTOSIZE);
  namedWindow("Scharr Filter (dy)", CV_WINDOW_AUTOSIZE);
  imshow("Scharr Filter (dx)", abs_grad_x);
  imshow("Scharr Filter (dy)", abs_grad_y);
}

void ImageFiltering::erotion(){
  Mat erosion_dst;
  Mat binarized_image; 
  threshold(grayscaleImage,binarized_image,127,255,THRESH_BINARY);
  int erosion_type = MORPH_RECT;
  int erosion_size = 5; //here increase for more erosion, reduce for less erosion
  Mat element = getStructuringElement( erosion_type,
                                       Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                       Point( erosion_size, erosion_size ) );

  /// Apply the erosion operation
  erode( binarized_image, erosion_dst, element );
  imshow("Original Binarized",binarized_image);
  imshow("Erotion 5x5", erosion_dst);

}

void ImageFiltering::dilation(){
  Mat dilation_dst;
  Mat binarized_image; 
  threshold(grayscaleImage,binarized_image,127,255,THRESH_BINARY);
  int dilation_type = MORPH_RECT;
  int dilation_size = 5; //here increase for more dilation, reduce for less dilation
  Mat element = getStructuringElement( dilation_type,
                                       Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                       Point( dilation_size, dilation_size ) );

  /// Apply the erosion operation
  dilate( binarized_image, dilation_dst, element );
  imshow("Original Binarized",binarized_image);
  imshow("Dilation 5x5", dilation_dst);
}

void ImageFiltering::endProgram()
{
  destroyAllWindows();
}