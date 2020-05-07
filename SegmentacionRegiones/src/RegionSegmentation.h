#include <iostream>
#include <string.h>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <list>
#include <queue>
#include <iterator>
using namespace std;
using namespace cv;

class Coord
{
public:
    int x;
    int y;
    Coord(int x, int y)
    {
        this->x = x;
        this->y = y;
    }
    Coord(){}
};

class RegionSegmentation
{
public:
    RegionSegmentation(Mat image, string screenName);
    void findRegions(int number_of_objects);

private:
    Mat *frame;
    Mat grayscaleImage;
    Mat binaryImage;
    Mat eroded;
    Mat dilated;
    Mat processed_image;
    Mat color_image;
    string screenName;
    Coord generateSeed();
    bool is_object_coord(Coord);
    void printImageInfo(int x, int y);
    Mat bgrToGray();
    void binaryFilter();
    void erotion();
    void dilation();
    Mat pipeline();
    int IMAGE_HEIGHT;
    int IMAGE_WIDTH;
};