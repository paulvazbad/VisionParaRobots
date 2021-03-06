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
#if WIN32
#include <windows.h>
#else
#include <X11/Xlib.h>
#endif
#include "ObjectAnalysis/ObjectAnalysis.h"
#include "Navigator/Navigator.h"

using namespace std;
using namespace cv;

class FindParkingSpace
{
public:
    FindParkingSpace(Mat parking_lot_image, string screenName);
    void showRobotTravel();
    void findPath(int entrance, bool direction);
    void calibrateMap(Mat map_image);
    void generateBaseImages();
private:
    void paint_in_map_to_display(vector<Point> path, Mat &map_to_display);
    void paint_in_map_to_display(double robot_radius, Point current_position, Mat &map_to_display);
    void validateFinalPoint(Point p);
    void colorFillSlot(Mat &map_to_display);
    void testPathAlgorithm();
    static bool compareContourAreas ( vector<Point> contour1, vector<Point> contour2 );
    vector<vector<Point>> contourApproximation(vector<vector<Point>> contours);
    void onMouse(int event, int x, int y);
    static void onMouse(int event, int x, int y, int, void *userdata);
    ObjectAnalysis objectAnalysis;
    string screenName;
    Navigator nav;
    Mat map, original, navigator_map, slots, map_helper;
    double robot;
    Point *finalPoint;
};