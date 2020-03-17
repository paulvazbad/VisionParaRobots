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
        void plotHist(double ColorValues[256], string canvasName);
        void GenerateRGBHist(const Mat &Image,double RGBValues[3][256]);
        void update();
        
    private:
        Mat *frame, hsvImage;
        Mat histograms[3];
        string screenName;
        Vec3b BGR_color, HSV_color;
        int epsilon; 

        void initializeMat(double RGBValues[3][256]);
        Mat hsvFilter();
        Mat bgrFilter();
        static void onMouse(int event, int x, int y, int, void* userdata);
        void onMouse(int event, int x, int y);
};