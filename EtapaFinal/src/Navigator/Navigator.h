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
    Coo coord;
    bool existent;
    bool passed;
    GridNode(Coo coo)
    {
        this->coord = coo;
        existent = true;
    }
    GridNode()
    {
        this->coord = Coo(-1,-1);
        this->existent = false;
        this->passed = false;
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
        for(int i1 = 0; i1<50; i1++){
            vector<GridNode> vector_grid_node;
            grid.push_back(vector_grid_node);
            for(int i2 = 0; i2<50; i2++){
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
    void findPath(int entrance, Coo finish, bool right);
    void startTravel();
private:
    int path_step;
    vector<Coo> path;
    vector<Coo> entrances;
    Mat map;
    Grid map_grid;
    vector<Coo> getPath();
    void displayPath();
    void displayCarPosition(Coo car_position, bool car_orientation);
    void nextStep();
    void scanMap();
};