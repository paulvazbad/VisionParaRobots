#include "Navigator.h"

Navigator::Navigator(Mat map, string screenName)
{
    this-> map = map.clone();
    resize(this->map, this->map, cv::Size(), 0.8, 0.8);
    cout<<map.cols<<" "<<map.rows<<endl;
    cout<<map.cols/50<<" "<<map.rows/50<<endl;
    cout<<"Scanning map"<<endl;
    scanMap();
}

void Navigator::scanMap()
{
    Mat binary_map = this->map.clone();
    Vec3b color_grid(0, 0, 255);
    for(int x = 0; x<50; x++){
        for(int y = 0; y<50; y++){
            if((int)binary_map.at<Vec3b>(Point(10+y*map.cols/50,10+x*map.rows/50))[0] == 255)
            {
                map_grid.grid[x][y].existent = true;
                map_grid.grid[x][y].coord.x = y;
                map_grid.grid[x][y].coord.y = x;
                binary_map.at<Vec3b>(10+x*map.rows/50, 10+y*map.cols/50)=color_grid;
            }
        }
    }

    //Find entrances
    Vec3b color_entrance(255, 0, 0);
    bool found = false;
    for(int x = 0; x<5 && found==false; x++){
        for(int y = 0; y<5; y++){
            if(map_grid.grid[x][y].existent)
            {
                entrances.push_back(Coo(x,y));
                binary_map.at<Vec3b>(10+x*map.rows/50, 10+y*map.cols/50)=color_entrance;
                found = true;
                break;
            }
        }
    }
    found = false;
    for(int x = 45; x<50 && found==false; x++){
        for(int y = 0; y<5; y++){
            if(map_grid.grid[x][y].existent)
            {
                entrances.push_back(Coo(x,y));
                binary_map.at<Vec3b>(10+x*map.rows/50, 10+y*map.cols/50)=color_entrance;
                found = true;
                break;
            }
        }
    }
    found = false;
    for(int x = 0; x<5 && found==false; x++){
        for(int y = 45; y<50; y++){
            if(map_grid.grid[x][y].existent)
            {
                entrances.push_back(Coo(x,y));
                binary_map.at<Vec3b>(10+x*map.rows/50, 10+y*map.cols/50)=color_entrance;
                found = true;
                break;
            }
        }
    }
    found = false;
    for(int x = 45; x<50 && found==false; x++){
        for(int y = 45; y<50; y++){
            if(map_grid.grid[x][y].existent)
            {
                entrances.push_back(Coo(x,y));
                binary_map.at<Vec3b>(10+x*map.rows/50, 10+y*map.cols/50)=color_entrance;
                found = true;
                break;
            }
        }
    }

    if(entrances.size() < 4)
    {
        cout<<"Entrances not found";
    }else{
        cout<<"Entrances found!";
        imshow("Grid", binary_map);
        waitKey(0);
    }
}


void Navigator::findPath(int entrance, Coo finish, bool right)
{
    Coo explorer;
    //entrance tiene
    explorer = entrances[entrance];
    bool reached = false;

    while(!reached)
    {
        if(right)
        {
            if(explorer.y < finish.y){
                if(map_grid.grid[explorer.x][explorer.y + 1].existent){
                    cout<<"in process"<<endl;
                }
            }
        }
    }
}

vector<Coo> Navigator::getPath(){
    return this->path;
}
// void Navigator::startTravel();
// void Navigator::displayCarPosition(Coord car_position, bool car_orientation);
// void Navigator::nextStep();