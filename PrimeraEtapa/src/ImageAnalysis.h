#include <iostream>
#include <string.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace std;
using namespace cv;
class ImageAnalysis
{
public:
    ImageAnalysis(Mat &image, string screenName);
    void plotRGBLines();
    void GenerateRGBHist(const Mat &Image);
    void update();
    void toggleHist();
private:
    Mat *frame, hsvImage;
    string screenName;
    Vec3b BGR_color, HSV_color;
    int epsilon;
    Mat b_hist, g_hist, r_hist;
    Mat rgb_gradients[3];
    void initializeMat();
    static const int HIST_HEIGHT = 400;
    static const int HIST_WIDTH = 512;
    static const int HIST_SIZE = 256;
    int bin_w;
    int current_hist;
    double windowsHeightRatio;
    int windowsVerticalPosition, windowsSecondColumnPosition, verticalOffset;
    Mat histImages[3];
    Mat hsvFilter();
    Mat bgrFilter();
    Mat binaryFilter();
    Mat bgrToYIQ();
    Mat bgrToHsv();
    static void onMouse(int event, int x, int y, int, void *userdata);
    void onMouse(int event, int x, int y);
    void getScreenResolution(int &width, int &height);
    void generateGradients();
};