#include "Navigator.h"

Navigator::Navigator(Mat map, string screenName)
{
    this-> map = map.clone();
    cout<<map.cols<<" "<<map.rows<<endl;
    cout<<map.cols/50<<" "<<map.rows/50<<endl;
    cout<<"Scanning map"<<endl;
    scanMap();
}

void Navigator::scanMap()
{
    Mat binary_map = this->map.clone();
    Vec3b color_grid(0, 0, 255);
    for(int i1 = 0; i1<50; i1++){
        for(int i2 = 0; i2<50; i2++){
            if((int)binary_map.at<Vec3b>(Point(10+i2*map.cols/50,10+i1*map.rows/50))[0] == 255)
            {
                map_grid.grid[i1][i2].existent = true;
                map_grid.grid[i1][i2].coord.x = i2;
                map_grid.grid[i1][i2].coord.y = i1;
                binary_map.at<Vec3b>(10+i1*map.rows/50, 10+i2*map.cols/50)=color_grid;
            }
        }
    }
    imshow("Grid", binary_map);
    waitKey(0);
}


// void Navigator::findPath(int entrance, Coord finish, bool right);
// void Navigator::startTravel();
// void Navigator::displayCarPosition(Coord car_position, bool car_orientation);
// void Navigator::nextStep();