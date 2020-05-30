#include "Navigator.h"

Navigator::Navigator(Mat map, string screenName)
{
    this-> map = map.clone();
    cout<<map.cols/50<<" "<<map.rows/50<<endl;
    cout<<"Scanning map"<<endl;
    scanMap();
}

void Navigator::scanMap()
{
    Mat color_map = this->map.clone();
    Mat binary_map = this->map.clone();
    threshold( binary_map, binary_map, 127, 255, CV_THRESH_BINARY );
    cout<<"Hey"<<endl;
    for(int i1 = 0; i1<50; i1++){
        for(int i2 = 0; i2<50; i2++){
            if(binary_map.at<uchar>(Point(i2*map.cols/50,i1*map.rows/50)) == 255)
            {
                cout<<"Valid node"<<endl;
                map_grid.grid[i1][i2].existent = true;
                map_grid.grid[i1][i2].coord.x = i1;
                map_grid.grid[i1][i2].coord.y = i2;
                color_map.at<Vec3b>(i1*map.cols/50,i2*map.rows/50).val[0]=0;
                color_map.at<Vec3b>(i1*map.cols/50,i2*map.rows/50).val[1]=0;
                color_map.at<Vec3b>(i1*map.cols/50,i2*map.rows/50).val[2]=255;
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