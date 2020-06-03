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
// #ifndef Coord
// #include "../common/common.h"
// #endif

using namespace std;
using namespace cv;

class Coo
{
public:
    int x;
    int y;
    Coo(int x, int y)
    {
        this->x = x;
        this->y = y;
    }
    Coo(){}
};

class GridNode
{
public:
    Point coord;
    bool existent;
    bool passed;
    GridNode(Point coo)
    {
        this->coord = coo;
        existent = true;
        this->passed = false;
    }
    GridNode()
    {
        this->coord = Point(-1,-1);
        this->existent = false;
        this->passed = true;
    }
};

class Grid
{
public:
    int x;
    int y;
    vector<vector<GridNode> > grid;

    Grid()
    {
        for(int i1 = 0; i1<20; i1++){
            vector<GridNode> vector_grid_node;
            grid.push_back(vector_grid_node);
            for(int i2 = 0; i2<20; i2++){
                GridNode grid_node;
                grid[i1].push_back(grid_node);
            }
        }
    }
};

class Navigator
{
public:
    Navigator(Mat map, string screenName);
    Navigator(){}
    void findPath(int entrance, Point finish, bool right);
    void startTravel();
    void findPathTesting(int entrance, Point finish, bool right);
    vector<Point> getPath();
private:
    int path_step;
    vector<Point> path;
    vector<Point> entrances;
    Mat map;
    Grid map_grid;
    void displayPath();
    void displayCarPosition(Point car_position, bool car_orientation);
    void nextStep();
    void scanMap();
};