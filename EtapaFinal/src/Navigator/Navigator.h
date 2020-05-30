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
#include "../common/common.h"
#if WIN32
#include <windows.h>
#else
#include <X11/Xlib.h>
#endif

using namespace std;
using namespace cv;

class GridNode
{
public:
    Coord coord;
    GridNode(Coord coo)
    {
        this->coord = coo;
    }
    GridNode(){}
};

class Grid
{
public:
    int x;
    int y;
    GridNode *grid;
    //construct grid
    Grid (int x, int y){
        this->x = x;
        this->y = y;
    } 
};

class Navigator
{
public:
    Navigator(Mat image, string screenName);

private:
    };