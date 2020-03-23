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

  namedWindow(screenName);
  setMouseCallback(screenName, onMouse, this);
  namedWindow("BGR Range");
  namedWindow("HSV Range");
  namedWindow("YIQ Range");
  createTrackbar("B", "BGR Range", &bgrRange[0], 255);
  createTrackbar("G", "BGR Range", &bgrRange[1], 255);
  createTrackbar("R", "BGR Range", &bgrRange[2], 255);
  createTrackbar("H", "HSV Range", &hsvRange[0], 255);
  createTrackbar("S", "HSV Range", &hsvRange[1], 255);
  createTrackbar("V", "HSV Range", &hsvRange[2], 255);
  createTrackbar("Y", "YIQ Range", &yiqRange[0], 255);
  createTrackbar("I", "YIQ Range", &yiqRange[1], 255);
  createTrackbar("Q", "YIQ Range", &yiqRange[2], 255);
  setRanges();

  cvtColor(*frame, hsvImage, CV_BGR2HSV);
  current_hist = 0;
  int width1, height1;
  height1 = frame->size().height;
  width1 = frame->size().width;
  int width2, height2;
  getScreenResolution(width2, height2);
  windowsVerticalPosition = height2 / 3;
  windowsHeightRatio = (double(windowsVerticalPosition) * 0.8) / height1;
  windowsSecondColumnPosition = windowsHeightRatio * width1 * 1.2;
  generateGradients();
}

void ImageAnalysis::setRanges()
{
  ifstream file("ranges.txt");

  if(file.fail()){
    std::cout << "File could not be opened\n";
    return;  
  }

  file >> bgrRange[0] >> bgrRange[1] >> bgrRange[2];
  file >> hsvRange[0] >> hsvRange[1] >> hsvRange[2];
  file >> yiqRange[0] >> yiqRange[1] >> yiqRange[2];
  
  setTrackbarPos("B", "BGR Range", bgrRange[0]);
  setTrackbarPos("G", "BGR Range", bgrRange[1]);
  setTrackbarPos("R", "BGR Range", bgrRange[2]);
  setTrackbarPos("H", "HSV Range", hsvRange[0]);
  setTrackbarPos("S", "HSV Range", hsvRange[1]);
  setTrackbarPos("V", "HSV Range", hsvRange[2]);
  setTrackbarPos("Y", "YIQ Range", yiqRange[0]);
  setTrackbarPos("I", "YIQ Range", yiqRange[1]);
  setTrackbarPos("Q", "YIQ Range", yiqRange[2]);
}

void ImageAnalysis::generateGradients()
{
  //RGB gradient generation
  for (int i = 0; i < 3; i++)
  {
    Mat row = Mat(50, HIST_WIDTH, CV_8UC3, Scalar(0, 0, 0));
    Mat hsv_row = Mat(50, HIST_WIDTH, CV_8UC3, Scalar(0, 0, 0));
    Mat yiq_row = Mat(50, HIST_WIDTH, CV_8UC3, Scalar(0, 0, 0));
    for (int x = 0; x < HIST_WIDTH; x++)
    {
      for (int y = 10; y < 50; y++)
      {
        row.at<Vec3b>(y, x)[i] = (x * HIST_SIZE) / HIST_WIDTH;
        if (i == 0)
        {
          //Changing the value of H
          hsv_row.at<Vec3b>(y, x)[0] = (x * 179) / HIST_WIDTH;
          hsv_row.at<Vec3b>(y, x)[1] = HSV_color[1];
          hsv_row.at<Vec3b>(y, x)[2] = HSV_color[2];
          //Change the value of Y
          yiq_row.at<Vec3b>(y, x)[0] = (x * 256) / HIST_WIDTH;
          yiq_row.at<Vec3b>(y, x)[1] = YIQ_color[1];
          yiq_row.at<Vec3b>(y, x)[2] = YIQ_color[2];
        }
        else if (i == 1)
        {
          //Chinging the value of S
          hsv_row.at<Vec3b>(y, x)[1] = (x * HIST_SIZE) / HIST_WIDTH;
          hsv_row.at<Vec3b>(y, x)[0] = HSV_color[0];
          hsv_row.at<Vec3b>(y, x)[2] = HSV_color[2];
          //Change the value of I
          yiq_row.at<Vec3b>(y, x)[1] = (x * 256) / HIST_WIDTH;
          yiq_row.at<Vec3b>(y, x)[0] = YIQ_color[0];
          yiq_row.at<Vec3b>(y, x)[2] = YIQ_color[2];
        }
        else
        {
          // Changing the value of V
          hsv_row.at<Vec3b>(y, x)[2] = (x * HIST_SIZE) / HIST_WIDTH;
          hsv_row.at<Vec3b>(y, x)[0] = HSV_color[0];
          hsv_row.at<Vec3b>(y, x)[1] = HSV_color[1];
          //Change the value of Q
          yiq_row.at<Vec3b>(y, x)[2] = (x * 256) / HIST_WIDTH;
          yiq_row.at<Vec3b>(y, x)[0] = YIQ_color[0];
          yiq_row.at<Vec3b>(y, x)[1] = YIQ_color[1];
        }
      }
    }
    rgb_gradients[i] = row;
    hsv_gradients[i] = hsv_row;
    yiq_gradients[i] = yiq_row;
  }
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

void ImageAnalysis::plotLines(Mat gradients[3], Vec3b colors, string histogramNames[3])
{
  //Calculate line position in hist
  int p_t_p[6];
  calculateMaxMinChannels(BGR_color, p_t_p[0], p_t_p[1], p_t_p[2], p_t_p[3], p_t_p[4], p_t_p[5]);
  //PLot line in hists
  //Copy of the hists
  Mat histImagesCopy[3];
  for (int i = 0; i < 3; i++)
  {
    //Add space for axis
    histImagesCopy[i] = histImages[i].clone();
    histImagesCopy[i].push_back(gradients[i]);
  }
  for (int i = 0; i < 6; i++)
  {
    line(histImagesCopy[int(i % 3)], Point(bin_w*p_t_p[i], 0), Point(bin_w*p_t_p[i], HIST_HEIGHT),
         Scalar(255, 255, 255), 1, 8, 0);
  }
  //Paint centers
  int center_line = bin_w * BGR_color[0];
  line(histImagesCopy[0], Point(center_line, 0), Point(center_line, HIST_HEIGHT),
       Scalar(255, 100, 255), 1, 8, 0);
      center_line = bin_w * BGR_color[1]; 
  line(histImagesCopy[1], Point(center_line, 0), Point(center_line, HIST_HEIGHT),
       Scalar(255, 100, 255), 1, 8, 0);
      center_line = bin_w * BGR_color[2];
  line(histImagesCopy[2], Point(center_line, 0), Point(center_line, HIST_HEIGHT),
       Scalar(255, 100, 255), 1, 8, 0);
  imshow(histogramNames[0], histImagesCopy[0]);
  imshow(histogramNames[1], histImagesCopy[1]);
  imshow(histogramNames[2], histImagesCopy[2]);
}

void ImageAnalysis::toggleHist(int keyPressed)
{
  if (keyPressed == 49)
  {
    if (current_hist == 1)
    {
      destroyWindow("Hue");
      destroyWindow("Saturation");
      destroyWindow("Value");
    }
    if (current_hist == 2)
    {

      destroyWindow("Y");
      destroyWindow("I");
      destroyWindow("Q");
    }
    current_hist = 0;
  }
  else if (keyPressed == 50)
  {
    if (current_hist == 0)
    {
      destroyWindow("Blue");
      destroyWindow("Red");
      destroyWindow("Green");
    }
    if (current_hist == 2)
    {

      destroyWindow("Y");
      destroyWindow("I");
      destroyWindow("Q");
    }
    current_hist = 1;
  }
  else if (keyPressed == 51)
  {
    if (current_hist == 0)
    {
      destroyWindow("Blue");
      destroyWindow("Red");
      destroyWindow("Green");
    }
    if (current_hist == 1)
    {
      destroyWindow("Hue");
      destroyWindow("Saturation");
      destroyWindow("Value");
    }
    current_hist = 2;
  }
}

void ImageAnalysis::GenerateHist(const Mat &Image, float ranges[3][2], const Scalar colors[])
{
  vector<Mat> planes;
  histImages[0] = Mat(HIST_HEIGHT, HIST_WIDTH, CV_8UC3, Scalar(0, 0, 0));
  histImages[1] = Mat(HIST_HEIGHT, HIST_WIDTH, CV_8UC3, Scalar(0, 0, 0));
  histImages[2] = Mat(HIST_HEIGHT, HIST_WIDTH, CV_8UC3, Scalar(0, 0, 0));
  split(Image, planes);
  int HIST_SIZE = 256;
  //float range[] = {0, 256}; //the upper boundary is exclusive
  const float *histRange_0 = {ranges[0]};
  const float *histRange_1 = {ranges[1]};
  const float *histRange_2 = {ranges[2]};
  bool uniform = true, accumulate = false;
  calcHist(&planes[0], 1, 0, Mat(), b_hist, 1, &HIST_SIZE, &histRange_0, uniform, accumulate);
  calcHist(&planes[1], 1, 0, Mat(), g_hist, 1, &HIST_SIZE, &histRange_1, uniform, accumulate);
  calcHist(&planes[2], 1, 0, Mat(), r_hist, 1, &HIST_SIZE, &histRange_2, uniform, accumulate);
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
         colors[0], 2, 8, 0);
    line(histImages[1], Point(bin_w * (i - 1), HIST_HEIGHT - cvRound(g_hist.at<float>(i - 1))),
         Point(bin_w * (i), HIST_HEIGHT - cvRound(g_hist.at<float>(i))),
         colors[1], 2, 8, 0);
    line(histImages[2], Point(bin_w * (i - 1), HIST_HEIGHT - cvRound(r_hist.at<float>(i - 1))),
         Point(bin_w * (i), HIST_HEIGHT - cvRound(r_hist.at<float>(i))),
         colors[2], 2, 8, 0);
  }
}

void ImageAnalysis::update()
{
  // color model conversions
  hsvImage = bgrToHsv();
  yiqImage = bgrToYIQ(*frame);
  // frame manipulation & updates
  Mat hsvFilteredImage = hsvFilter();
  Mat bgrFilteredImage = bgrFilter();
  Mat binaryConvertedImage = binaryFilter();
  Mat yiqFilteredImage = yiqFilter();

  //update histograms
  if (current_hist == 0)
  {
    //RGB hist
    float ranges[3][2] = {{0, 256}, {0, 256}, {0, 256}};
    Scalar rgb_colors[3] = {Scalar(255, 0, 0), Scalar(0, 255, 0), Scalar(0, 0, 255)};
    GenerateHist(*frame, ranges, rgb_colors);
    string histogramNames[3] = {"Blue", "Green", "Red"};
    plotLines(rgb_gradients, BGR_color, histogramNames);
  }
  else if (current_hist == 1)
  {
    //HSV hist
    float ranges[3][2] = {{0, 180}, {0, 256}, {0, 256}};
    Scalar hsv_colors[3] = {Scalar(255, 255, 0), Scalar(0, 255, 255), Scalar(255, 0, 255)};
    GenerateHist(hsvImage, ranges, hsv_colors);
    generateGradients();
    string histogramNames[3] = {"Hue", "Saturation", "Value"};
    cvtColor(hsv_gradients[0], hsv_gradients[0], CV_HSV2BGR);
    cvtColor(hsv_gradients[1], hsv_gradients[1], CV_HSV2BGR);
    cvtColor(hsv_gradients[2], hsv_gradients[2], CV_HSV2BGR);
    plotLines(hsv_gradients, HSV_color, histogramNames);
  }
  else if (current_hist == 2)
  {
    //YIQ hist
    float ranges[3][2] = {{0, 256}, {0, 256}, {0, 256}};
    Scalar yiq_colors[3] = {Scalar(255, 255, 180), Scalar(180, 255, 255), Scalar(255, 180, 255)};
    GenerateHist(yiqImage, ranges, yiq_colors);
    generateGradients();
    string histogramNames[3] = {"Y", "I", "Q"};
    IplImage temp = yiq_gradients[0];
    yiq_gradients[0] = cvarrToMat(convertImageYIQtoRGB(temp));
    temp = yiq_gradients[1];
    yiq_gradients[1] = cvarrToMat(convertImageYIQtoRGB(temp));
    temp = yiq_gradients[2];
    yiq_gradients[2] = cvarrToMat(convertImageYIQtoRGB(temp));
    plotLines(yiq_gradients, YIQ_color, histogramNames);
  }

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

  imshow("YIQ Filtered", yiqFilteredImage);

  moveWindow("Binary Converted", windowsSecondColumnPosition, windowsVerticalPosition + verticalOffset);
  resize(binaryConvertedImage, outImg, cv::Size(), windowsHeightRatio, windowsHeightRatio);
  imshow("Binary Converted", outImg);

  moveWindow("HSV Converted", 0, windowsVerticalPosition * 2 + verticalOffset);
  resize(hsvImage, outImg, cv::Size(), windowsHeightRatio, windowsHeightRatio);
  imshow("HSV Converted", outImg);

  moveWindow("YIQ Converted", windowsSecondColumnPosition, windowsVerticalPosition * 2 + verticalOffset);
  resize(yiqImage, outImg, cv::Size(), windowsHeightRatio, windowsHeightRatio);
  imshow("YIQ Converted", outImg);
}

Mat ImageAnalysis::hsvFilter()
{
  Mat result = Mat::zeros((*frame).size(), (*frame).type()), mask;
  int hMin, hMax, sMin, sMax, vMin, vMax;
  calculateMaxMinChannels(HSV_color, hMin, sMin, vMin, hMax, sMax, vMax);

  // Updates mask values with the corresponding H,S,V limits
  inRange(hsvImage, Scalar(hMin, sMin, vMin), Scalar(hMax, sMax, vMax), mask);

  // Combines the mask with the original image, as result we get only the filtered colors
  (*frame).copyTo(result, mask);

  return result;
}

void ImageAnalysis::calculateMaxMinChannels(Vec3b &color, int &bMin, int &gMin, int &rMin, int &bMax, int &gMax, int &rMax)
{
  int epsilon[3];

  if (&color == &BGR_color)
    memcpy(epsilon, bgrRange, 3 * sizeof *bgrRange);
  else if (&color == &HSV_color)
    memcpy(epsilon, hsvRange, 3 * sizeof *hsvRange);
  else if (&color == &YIQ_color)
    memcpy(epsilon, yiqRange, 3 * sizeof *yiqRange);
  else
    cout << "ERROR IN CalculateMaxMinChannels" << endl;

  bMin = max(color[0] - epsilon[0], 0);
  gMin = max(color[1] - epsilon[1], 0);
  rMin = max(color[2] - epsilon[2], 0);
  bMax = min(color[0] + epsilon[0], 255);
  gMax = min(color[1] + epsilon[1], 255);
  rMax = min(color[2] + epsilon[2], 255);
}

Mat ImageAnalysis::bgrFilter()
{
  Mat result = Mat::zeros((*frame).size(), (*frame).type()), mask;
  int bMin, bMax, gMin, gMax, rMin, rMax;
  calculateMaxMinChannels(BGR_color, bMin, gMin, rMin, bMax, gMax, rMax);
  // Updates mask values with the corresponding B,G,R limits
  inRange(*frame, Scalar(bMin, gMin, rMin), Scalar(bMax, gMax, rMax), mask);

  // Combines the mask with the original image, as result we get only the filtered colors
  (*frame).copyTo(result, mask);

  return result;
}

Mat ImageAnalysis::yiqFilter()
{
  Mat result = Mat::zeros((*frame).size(), (*frame).type()), mask;
  int yMin, yMax, iMin, iMax, qMin, qMax;
  calculateMaxMinChannels(YIQ_color, yMin, iMin, qMin, yMax, iMax, qMax);

  // Updates mask values with the corresponding H,S,V limits
  inRange(yiqImage, Scalar(yMin, iMin, qMin), Scalar(yMax, iMax, qMax), mask);

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
Mat ImageAnalysis::bgrToYIQ(Mat frame)
{
  Mat result = frame.clone();

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

IplImage *ImageAnalysis::convertImageYIQtoRGB(const IplImage imageYIQ)
{
  float fY, fI, fQ;
  float fR, fG, fB;
  const float FLOAT_TO_BYTE = 255.0f;
  const float BYTE_TO_FLOAT = 1.0f / FLOAT_TO_BYTE;
  const float MIN_I = -0.5957f;
  const float MIN_Q = -0.5226f;
  const float Y_TO_FLOAT = 1.0f / 255.0f;
  const float I_TO_FLOAT = -2.0f * MIN_I / 255.0f;
  const float Q_TO_FLOAT = -2.0f * MIN_Q / 255.0f;
  const IplImage *image_p = &imageYIQ;
  // Create a blank RGB image
  IplImage *imageRGB = cvCreateImage(cvGetSize(image_p), 8, 3);

  int h = imageYIQ.height;              // Pixel height
  int w = imageYIQ.width;               // Pixel width
  int rowSizeYIQ = imageYIQ.widthStep;  // Size of row in bytes, including extra padding.
  char *imYIQ = imageYIQ.imageData;     // Pointer to the start of the image pixels.
  int rowSizeRGB = imageRGB->widthStep; // Size of row in bytes, including extra padding.
  char *imRGB = imageRGB->imageData;    // Pointer to the start of the image pixels.
  for (int y = 0; y < h; y++)
  {
    for (int x = 0; x < w; x++)
    {
      // Get the YIQ pixel components
      uchar *pYIQ = (uchar *)(imYIQ + y * rowSizeYIQ + x * 3);
      int bY = *(uchar *)(pYIQ + 0); // Y component
      int bI = *(uchar *)(pYIQ + 1); // I component
      int bQ = *(uchar *)(pYIQ + 2); // Q component

      // Convert from 8-bit integers to floats
      fY = (float)bY * Y_TO_FLOAT;
      fI = (float)bI * I_TO_FLOAT + MIN_I;
      fQ = (float)bQ * Q_TO_FLOAT + MIN_Q;
      // Convert from YIQ to RGB
      // where R,G,B are 0-1, Y is 0-1, I is -0.5957 to +0.5957, Q is -0.5226 to +0.5226.
      fR = fY + 0.9563 * fI + 0.6210 * fQ;
      fG = fY - 0.2721 * fI - 0.6474 * fQ;
      fB = fY - 1.1070 * fI + 1.7046 * fQ;
      // Convert from floats to 8-bit integers
      int bR = (int)(fR * FLOAT_TO_BYTE);
      int bG = (int)(fG * FLOAT_TO_BYTE);
      int bB = (int)(fB * FLOAT_TO_BYTE);

      // Clip the values to make sure it fits within the 8bits.
      if (bR > 255)
        bR = 255;
      if (bR < 0)
        bR = 0;
      if (bG > 255)
        bG = 255;
      if (bG < 0)
        bG = 0;
      if (bB > 255)
        bB = 255;
      if (bB < 0)
        bB = 0;

      // Set the RGB pixel components. NOTE that OpenCV stores RGB pixels in B,G,R order.
      uchar *pRGB = (uchar *)(imRGB + y * rowSizeRGB + x * 3);
      *(pRGB + 0) = bB; // B component
      *(pRGB + 1) = bG; // G component
      *(pRGB + 2) = bR; // R component
    }
  }
  return imageRGB;
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
    YIQ_color = (yiqImage).at<Vec3b>(Point(x, y));
    cout << "B = " << (int)BGR_color[0] << endl;
    cout << "G = " << (int)BGR_color[1] << endl;
    cout << "R = " << (int)BGR_color[2] << endl;
    cout << "H = " << (int)HSV_color[0] << endl;
    cout << "S = " << (int)HSV_color[1] << endl;
    cout << "V = " << (int)HSV_color[2] << endl;
    cout << "Y = " << (int)YIQ_color[0] << endl;
    cout << "I = " << (int)YIQ_color[1] << endl;
    cout << "Q = " << (int)YIQ_color[2] << endl;
    break;
  case CV_EVENT_MOUSEMOVE:
    break;
  case CV_EVENT_LBUTTONUP:
    break;
  }
}

void ImageAnalysis::saveRanges()
{
  ofstream outFile("ranges.txt");

  string file;

  file += bgrRange[0] + " " + bgrRange[1];
  file += " " + bgrRange[2];
  file += "\n";
  file += hsvRange[0] + " " + hsvRange[1];
  file += " " + hsvRange[2];
  file += "\n";
  file += yiqRange[0] + " " + yiqRange[1];
  file += " " + yiqRange[2];
  file += "\n";

  outFile << file;
  outFile.close();
}