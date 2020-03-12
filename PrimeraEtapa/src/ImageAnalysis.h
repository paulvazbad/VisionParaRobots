#include <iostream>
#include <string.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace std;
using namespace cv;
class ImageAnalysis{
    public:
        void plotHist(double ColorValues[256], string canvasName);
        void GenerateRGBHist(const Mat &Image,double RGBValues[3][256]);
        
    private:
        Mat frame;
        Mat histograms[3];
        void initializeMat(double RGBValues[3][256]);


};