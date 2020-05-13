#include <iostream>
#include <string.h>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <list>
#include <queue>
#include <iterator>
#include <time.h>  
#include <vector>
#include <cmath>
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

struct InformationOfRegionFound
{
    int size;
    vector<vector<long>> ordinary_moments;
};

class ObjectAnalysis
{
public:
    ObjectAnalysis(Mat image, string screenName);
    void findRegions(int number_of_objects);

private:
    Mat *frame;
    Mat grayscaleImage;
    Mat color_image;
    string screenName;
    Coord generateSeed();
    bool is_object_coord(Coord);
    void printImageInfo(int x, int y);
    void save_partial_results(time_t &last_time, time_t &curr_time, double seconds, int number_of_seed);
    Mat bgrToGray();
    Mat grayTobgr(Mat);
    InformationOfRegionFound grow_region_found(queue<Coord>&);
    void paint_and_append_object_neighbors(Coord,Vec3b,queue<Coord>&);
    void add_to_ordinary_moments(InformationOfRegionFound&,Coord);
    void print_ordinary_moments(InformationOfRegionFound);
    int IMAGE_HEIGHT;
    int IMAGE_WIDTH;
    const static int MAX_ORDINARY_MOMENT_P = 2, MAX_ORDINARY_MOMENT_Q = 2;
};