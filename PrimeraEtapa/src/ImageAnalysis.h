#include <iostream>
#include <string.h>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace std;
using namespace cv;
class ImageAnalysis
{
public:
    ImageAnalysis(Mat &image, string screenName);
    void plotLines(Mat gradients[3],Vec3b &colors,string histogramNames[3]);
    void GenerateHist(const Mat &Image, float ranges[3][2], const Scalar colors[]);
    void update();
    void toggleHist(int);
    void saveRanges();
    void endProgram();

private:
    Mat *frame, hsvImage, yiqImage;
    int hsvRange[3], yiqRange[3], bgrRange[3];
    string screenName;
    Vec3b BGR_color, HSV_color, YIQ_color;
    Mat b_hist, g_hist, r_hist;
    Mat rgb_gradients[3];
    Mat hsv_gradients[3];
    Mat yiq_gradients[3];
    void initializeMat();
    static const int HIST_HEIGHT = 400;
    static const int HIST_WIDTH = 512;
    static const int HIST_SIZE = 256;
    int bin_w;
    int current_hist;
    double windowsHeightRatio;
    int windowsVerticalPosition, windowsSecondColumnPosition;
    int windowFullHorizontalSize, windowFullVerticalSize, verticalOffset, horizontalOffset;
    double histogramSizeRatioVertical, histogramSizeRatioHorizontal;
    Mat histImages[3];
    Mat hsvFilter();
    Mat bgrFilter();
    Mat binaryFilter();
    Mat bgrToYIQ(Mat);
    Mat bgrToHsv();
    Mat bgrToGray();
    Mat yiqFilter();
    static void onMouse(int event, int x, int y, int, void *userdata);
    void onMouse(int event, int x, int y);
    void getScreenResolution(int &width, int &height);
    void generateGradients();
    void calculateMaxMinChannels(Vec3b &color, int &bMin, int &bMax, int &gMin, int &gMax, int &rMin, int &rMax);
    void setRanges();
    IplImage* convertImageYIQtoRGB(const IplImage imageYIQ);
};