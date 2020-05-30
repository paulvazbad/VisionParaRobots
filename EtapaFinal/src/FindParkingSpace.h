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
    void startTravel(Mat map_image);
    void findPath(Mat map_image);
    void calibrateMap(Mat map_image);
private:
    ObjectAnalysis objectAnalysis;
    Navigator nav;
    Mat map;
};