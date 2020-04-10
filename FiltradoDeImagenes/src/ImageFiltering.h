#include <iostream>
#include <string.h>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace std;
using namespace cv;
class ImageFiltering
{
public:
    ImageFiltering(Mat &image, string screenName);
    void plotLines(Mat gradients[3],Vec3b &colors,string histogramNames[3]);
    void GenerateHist(const Mat &Image, float ranges[3][2], const Scalar colors[]);
    void update();
    void endProgram();

private:
    Mat *frame, hsvImage, yiqImage;
    string screenName;
    static const int HIST_HEIGHT = 400;
    static const int HIST_WIDTH = 512;
    static const int HIST_SIZE = 256;
    double windowsHeightRatio;
    int windowsVerticalPosition, windowsSecondColumnPosition;
    int windowFullHorizontalSize, windowFullVerticalSize, verticalOffset, horizontalOffset;
    double histogramSizeRatioVertical, histogramSizeRatioHorizontal;
    Mat grayscaleImage;
    Mat binaryFilter();
    Mat bgrToGray();
    void onMouse(int event, int x, int y);
    static void onMouse(int event, int x, int y, int, void *userdata);
    void getScreenResolution(int &width, int &height);
    void printImageInfo(int x, int y);
    //Low Pass Filters
    void medianFilter();
    void averageFilter();
    void gaussianFilter();
    void laplaceFilter();
    void logFilter();
    void edgeDetectionFilter();
};