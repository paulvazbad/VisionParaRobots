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
    GridNode(Coo coo)
    {
        this->coord = coo;
        existent = true;
    }
    GridNode()
    {
        this->coord = Coo(-1,-1);
        this->existent = false;
    }
};

class Grid
{
public:
    int x;
    int y;
    vector<vector<GridNode> > grid;

    Grid(){}
    void fillEmpty(int x, int y){
        cout<<"U ok?"<<endl;
        this->x = x;
        this->y = y;
        cout<<"About to go into loop"<<endl;
        for(int i1 = 0; i1<x; i1++){
            vector<GridNode> vector_grid_node;
            grid.push_back(vector_grid_node);
            for(int i2 = 0; i2<y; i2++){
                GridNode grid_node;
                grid[x].push_back(grid_node);
            }
        }
        cout<<"Outside loop"<<endl;
    }
};

class Navigator
{
public:
    Navigator(Mat map, string screenName);
    Navigator(){}
    void findPath(int entrance, Coo finish);
    void startTravel();
private:
    int path_step;
    vector<GridNode> path;
    Mat map;
    Grid map_grid;
    void generatePath(bool right);
    void displayCarPosition(Coo car_position, bool car_orientation);
    void nextStep();
    void scanMap();
};