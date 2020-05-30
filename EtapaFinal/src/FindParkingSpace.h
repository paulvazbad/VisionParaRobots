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
    FindParkingSpace();
    void startTravel();
    void findPath();
    void calibrateMap(Mat map_image, string screenName);
private:
    ObjectAnalysis objectAnalysis;
    Navigator nav;
};