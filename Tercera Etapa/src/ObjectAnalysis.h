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
    double angle;
    int cx, cy;
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
    void calculate_median(){
        median_ph1 /= entries_in_dataset;
        median_ph2 /= entries_in_dataset; 
    }
    void calculate_variance(){
        double squared_differences_ph1= 0,squared_differences_ph2= 0;
        for(int i=0; i<ph1s.size(); i++){
            try{
                squared_differences_ph1 += pow(median_ph1 - ph1s[i], 2);
                squared_differences_ph2 += pow(median_ph2 - ph2s[i], 2); 
            }
            catch(...){
                cout<<"different # of ph1s or ph2s for "<<name_of_object<<endl;
            }
        }
        this->variance_ph1 = squared_differences_ph1 / entries_in_dataset;
        this->variance_ph2 = squared_differences_ph2 / entries_in_dataset;
    }

    void set_variance(long double variance_ph1, long double variance_ph2){
        this->variance_ph1 = variance_ph1;
        this->variance_ph2 = variance_ph2;
    }
};

class ObjectAnalysis
{
public:
    ObjectAnalysis(Mat image, string screenName);
    void findRegions(const int number_of_objects,const int SEED_LIMIT);
    void train(string name_of_object);
    void captureTrainData(Mat image);
    void filterImage(Mat image);

private:
    Mat *frame, filteredImage, hsvImage;
    Mat grayscaleImage;
    Mat color_image;
    string screenName;
    Vec3b HSV_color;
    vector<InformationOfRegionFound> regionsFound;
    vector<ObjectInformation> objectModels;
    int hsvRange[3];
    Coord generateSeed();
    bool is_object_coord(Coord);
    void displayResult(InformationOfRegionFound inf, int combination);
    void printImageInfo(int x, int y);
    void save_partial_results(time_t &last_time, time_t &curr_time, double seconds, int number_of_seed);
    void save_moments_to_dataset(string);
    void recalculate_models();
    void calculateMaxMinChannels(Vec3b &color, int &bMin, int &gMin, int &rMin, int &bMax, int &gMax, int &rMax);
    void dilation();
    void erotion();
    void bgrToHsv();
    void hsvFilter();
    void onMouse(int event, int x, int y);
    void getScreenResolution(int &width, int &height);
    static void onMouse(int event, int x, int y, int, void *userdata);
    Mat bgrToGray();
    Mat grayTobgr(Mat); 
    InformationOfRegionFound grow_region_found(queue<Coord>&);
    void paint_and_append_object_neighbors(Coord,Vec3b,queue<Coord>&);
    void add_to_ordinary_moments(InformationOfRegionFound&,Coord);
    void calculate_moments(InformationOfRegionFound&);
    void print_moments(InformationOfRegionFound);
    void draw_moments(InformationOfRegionFound, int hyp);
    void load_dataset_into_hashmap(unordered_map<string,ObjectInformation>&);
    void update_median_variance(unordered_map<string,ObjectInformation>&);
    long double get_normalized_moments(InformationOfRegionFound&, long centralizedMoment, int p, int q);
    void read_model();
    string match_shape(InformationOfRegionFound);
    long double eucladian_distance(long double x1, long double x2, long double y1, long double y2);
    int IMAGE_HEIGHT;
    int IMAGE_WIDTH;
    const static int MAX_ORDINARY_MOMENT_P = 2, MAX_ORDINARY_MOMENT_Q = 2;
};