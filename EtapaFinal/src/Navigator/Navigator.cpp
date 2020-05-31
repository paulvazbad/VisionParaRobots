#include "Navigator.h"

Navigator::Navigator(Mat map, string screenName)
{
    this-> map = map.clone();
    resize(this->map, this->map, cv::Size(), 0.8, 0.8);
    cout<<map.cols<<" "<<map.rows<<endl;
    cout<<map.cols/50<<" "<<map.rows/50<<endl;
    cout<<"Scanning map"<<endl;
    scanMap();
    cout<<"Finding path"<<endl;
    findPath(1,Coo(0,0),false);
}

void Navigator::scanMap()
{
    Mat binary_map = this->map.clone();
    Vec3b color_grid(0, 0, 255);
    for(int x = 0; x<50; x++){
        for(int y = 0; y<50; y++){
            if((int)binary_map.at<Vec3b>(Point(10+y*map.cols/50,10+x*map.rows/50))[0] == 255)
            {
                //cout<<"Point found"<<x<<" "<<y<<endl;
                map_grid.grid[x][y].existent = true;
                map_grid.grid[x][y].passed = false;
                // map_grid.grid[x][y].coord.x = y;
                // map_grid.grid[x][y].coord.y = x;
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
        cout<<"Entrances found!"<<endl;
        imshow("Grid", binary_map);
    }
}


void Navigator::findPath(int entrance, Coo finish, bool right)
{
    Coo explorer;
    Coo finish_debug(22, 5);
    //Se usan las coordenadas [indices] de los nodos
    explorer = entrances[entrance];
    bool reached = false;
    bool changed = false;
    Vec3b color_path(255, 0, 255);
    Point previous = Point(10+(explorer.y)*map.cols/50, 10+(explorer.x)*map.rows/50);
    map_grid.grid[explorer.x][explorer.y].passed = true;
    cout<<"Punto de partida: "<<explorer.x<<" "<<explorer.y<<endl;
    cout<<"Punto final: "<<finish_debug.x<<" "<<finish_debug.y<<endl;

    if(right)
    {
        bool go_down = explorer.x < finish_debug.x;
        while(!reached)
        {
            changed=false;
            while(explorer.x < finish_debug.x && !map_grid.grid[explorer.x+1][explorer.y].passed)
            {    //go down
                cout<<"Go down"<<endl;
                explorer.x++;
                changed = true;
            }
            path.push_back(explorer);
            line(this->map, previous, Point(0+(explorer.y)*map.cols/50, 10+(explorer.x)*map.rows/50), Scalar(0, 0, 255), 2);
            previous = Point(10+(explorer.y)*map.cols/50, 10+(explorer.x)*map.rows/50);

            while(explorer.x > finish_debug.x && !map_grid.grid[explorer.x-1][explorer.y].passed)
            {    //go up
                cout<<"Go up"<<endl;
                explorer.x--;
                changed = true;
            }
            path.push_back(explorer);
            line(this->map, previous, Point(0+(explorer.y)*map.cols/50, 10+(explorer.x)*map.rows/50), Scalar(0, 0, 255), 2);
            previous = Point(10+(explorer.y)*map.cols/50, 10+(explorer.x)*map.rows/50);
            while(explorer.y > finish_debug.y && !map_grid.grid[explorer.x][explorer.y-1].passed)
            {   //got left
                cout<<"Go left"<<endl;
                explorer.y--;
                changed = true;
            }
            path.push_back(explorer);
            line(this->map, previous, Point(0+(explorer.y)*map.cols/50, 10+(explorer.x)*map.rows/50), Scalar(0, 0, 255), 2);
            previous = Point(10+(explorer.y)*map.cols/50, 10+(explorer.x)*map.rows/50);
            while(explorer.y < finish_debug.y && !map_grid.grid[explorer.x][explorer.y+1].passed)
            {   //got right
                cout<<"Go right"<<endl;
                explorer.y++;
                changed = true;
            }
            path.push_back(explorer);
            line(this->map, previous, Point(0+(explorer.y)*map.cols/50, 10+(explorer.x)*map.rows/50), Scalar(0, 0, 255), 2);
            previous = Point(10+(explorer.y)*map.cols/50, 10+(explorer.x)*map.rows/50);
            if(!changed){
                cout<<"Go up/down"<<endl;
                (go_down)?explorer.x++:explorer.x--;
            }
            
            map_grid.grid[explorer.x][explorer.y].passed = true;
            previous = Point(10+(explorer.y)*map.cols/50, 10+(explorer.x)*map.rows/50);
            path.push_back(explorer);
            line(this->map, previous, Point(0+(explorer.y)*map.cols/50, 10+(explorer.x)*map.rows/50), Scalar(0, 0, 255), 2);

            if(explorer.x == finish_debug.x && explorer.y == finish_debug.y){
                cout<<"Reached"<<endl;
                reached = true;
                break;
            }
        }
    }
    else
    {
        bool go_left = explorer.y > finish_debug.y;
        while(!reached)
        {
            changed=false;
            previous = Point(10+(explorer.y)*map.cols/50, 10+(explorer.x)*map.rows/50);
            while(explorer.y > finish_debug.y && !map_grid.grid[explorer.x][explorer.y-1].passed)
            {   //got left
                cout<<"Go left"<<endl;
                explorer.y--;
                changed = true;
            }
            path.push_back(explorer);
            line(this->map, previous, Point(0+(explorer.y)*map.cols/50, 10+(explorer.x)*map.rows/50), Scalar(0, 0, 255), 2);
            previous = Point(10+(explorer.y)*map.cols/50, 10+(explorer.x)*map.rows/50);
            while(explorer.y < finish_debug.y && !map_grid.grid[explorer.x][explorer.y+1].passed)
            {   //got right
                cout<<"Go right"<<endl;
                explorer.y++;
                changed = true;
            }
            path.push_back(explorer);
            line(this->map, previous, Point(0+(explorer.y)*map.cols/50, 10+(explorer.x)*map.rows/50), Scalar(0, 0, 255), 2);
            previous = Point(10+(explorer.y)*map.cols/50, 10+(explorer.x)*map.rows/50);

            while(explorer.x < finish_debug.x && !map_grid.grid[explorer.x+1][explorer.y].passed)
            {    //go down
                cout<<"Go down"<<endl;
                explorer.x++;
                changed = true;
            }
            path.push_back(explorer);
            line(this->map, previous, Point(0+(explorer.y)*map.cols/50, 10+(explorer.x)*map.rows/50), Scalar(0, 0, 255), 2);
            previous = Point(10+(explorer.y)*map.cols/50, 10+(explorer.x)*map.rows/50);

            while(explorer.x > finish_debug.x && !map_grid.grid[explorer.x-1][explorer.y].passed)
            {    //go up
                cout<<"Go up"<<endl;
                explorer.x--;
                changed = true;
            }
            path.push_back(explorer);
            line(this->map, previous, Point(0+(explorer.y)*map.cols/50, 10+(explorer.x)*map.rows/50), Scalar(0, 0, 255), 2);
            previous = Point(10+(explorer.y)*map.cols/50, 10+(explorer.x)*map.rows/50);

            if(!changed){
                cout<<"Go left/right"<<endl;
                (go_left)?explorer.y--:explorer.y++;
            }
            path.push_back(explorer);
            map_grid.grid[explorer.x][explorer.y].passed = true;
            line(this->map, previous, Point(0+(explorer.y)*map.cols/50, 10+(explorer.x)*map.rows/50), Scalar(0, 0, 255), 2);
            previous = Point(10+(explorer.y)*map.cols/50, 10+(explorer.x)*map.rows/50);

            if(explorer.x == finish_debug.x && explorer.y == finish_debug.y){
                cout<<"Reached"<<endl;
                reached = true;
                break;
            }
        }
    }
    imshow("Path",this->map);
    waitKey(0);
}

vector<Coo> Navigator::getPath(){
    return this->path;
}
// void Navigator::startTravel();
// void Navigator::displayCarPosition(Coord car_position, bool car_orientation);
// void Navigator::nextStep();