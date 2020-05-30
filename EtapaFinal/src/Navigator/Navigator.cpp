#include "Navigator.h"

Navigator::Navigator(Mat map, string screenName)
{
    this-> map = map.clone();
    cout<<"Generating grid"<<endl;
    this->map_grid.fillEmpty(map.cols/100, map.rows/100);
    cout<<"Scanning map"<<endl;
    scanMap();
}

void Navigator::scanMap()
{
    Mat color_map = this->map.clone();
    Mat binary_map = this->map.clone();
    threshold( binary_map, binary_map, 127, 255, CV_THRESH_BINARY );
    for(int i1 = 0; i1<map_grid.x; i1++){
        for(int i2 = 0; i2<map_grid.y; i2++){
            if(binary_map.at<uchar>(Point(i1,i2)) == 255)
            {
                map_grid.grid[i1][i2].existent = true;
                map_grid.grid[i1][i2].coord.x = i1;
                map_grid.grid[i1][i2].coord.y = i2;
                color_map.at<Vec3b>(i1,i2).val[0]=0;
                color_map.at<Vec3b>(i1,i2).val[1]=0;
                color_map.at<Vec3b>(i1,i2).val[2]=255;
            }
        }
    }
    imshow("Grid", color_map);
    imshow("Binary map", binary_map);
}


// void Navigator::findPath(int entrance, Coord finish);
// void Navigator::startTravel();
// void Navigator::generatePath(bool right);
// void Navigator::displayCarPosition(Coord car_position, bool car_orientation);
// void Navigator::nextStep();