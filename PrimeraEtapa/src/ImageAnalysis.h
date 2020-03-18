#include <iostream>
#include <string.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace std;
using namespace cv;
class ImageAnalysis{
    public:
        ImageAnalysis(Mat &image, string screenName);
        void plotHist();
        void GenerateRGBHist(const Mat &Image);
        void update();
        
    private:
        Mat *frame, hsvImage;
        string screenName;
        Vec3b BGR_color, HSV_color;
        int epsilon; 
        Mat b_hist,g_hist,r_hist;
        void initializeMat();
        static const int HIST_HEIGHT = 400;
        static const int HIST_WIDTH = 512;
        static const int HIST_SIZE = 256;
        Mat histImages[3];
        Mat hsvFilter();
        Mat bgrFilter();
        Mat binaryFilter();
        static void onMouse(int event, int x, int y, int, void* userdata);
        void onMouse(int event, int x, int y);
};