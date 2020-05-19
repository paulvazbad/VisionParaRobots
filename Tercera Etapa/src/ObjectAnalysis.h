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
#include <unordered_map>

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
    vector<vector<long> > ordinary_moments;
    long u20, u02, u11;
    long double n20, n02, n11;
    long double ph1, ph2;
};
class ObjectInformation{
public:
    string name_of_object;
    vector<double> ph1s; 
    vector<double> ph2s;
    long double median_ph1;
    long double median_ph2;
    long double variance_ph1;
    long double variance_ph2;
    long double entries_in_dataset;
    ObjectInformation(string name, long double phi1,long double phi2){
        name_of_object = name;
        ph1s.push_back(phi1);
        ph2s.push_back(phi2);
        entries_in_dataset = 0;
        median_ph1 = phi1;
        median_ph2 = phi2;
    }
    ObjectInformation(){
        entries_in_dataset = 0;
    }
};

class ObjectAnalysis
{
public:
    ObjectAnalysis(Mat image, string screenName);
    void findRegions(int number_of_objects);
    void train(string name_of_object);

private:
    Mat *frame;
    Mat grayscaleImage;
    Mat color_image;
    string screenName;
    vector<InformationOfRegionFound> regionsFound;
    Coord generateSeed();
    bool is_object_coord(Coord);
    void printImageInfo(int x, int y);
    void save_partial_results(time_t &last_time, time_t &curr_time, double seconds, int number_of_seed);
    void save_moments_to_dataset(string);
    void recalculate_models();
    Mat bgrToGray();
    Mat grayTobgr(Mat);
    InformationOfRegionFound grow_region_found(queue<Coord>&);
    void paint_and_append_object_neighbors(Coord,Vec3b,queue<Coord>&);
    void add_to_ordinary_moments(InformationOfRegionFound&,Coord);
    void calculate_moments(InformationOfRegionFound&);
    void print_moments(InformationOfRegionFound);
    void load_dataset_into_hashmap(unordered_map<string,ObjectInformation>&);
    void calculate_median_variance(unordered_map<string,ObjectInformation>&);
    long double get_normalized_moments(InformationOfRegionFound&, long centralizedMoment, int p, int q);
    int IMAGE_HEIGHT;
    int IMAGE_WIDTH;
    const static int MAX_ORDINARY_MOMENT_P = 2, MAX_ORDINARY_MOMENT_Q = 2;
};