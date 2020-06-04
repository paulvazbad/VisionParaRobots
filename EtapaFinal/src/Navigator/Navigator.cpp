#include "Navigator.h"

Navigator::Navigator(Mat map, string screenName)
{
    this-> map = map.clone();
    // cout<<"Map provided: "<<this->map.cols<<endl;
    // cout<<"Scanning map"<<endl;
    scanMap();
}

void Navigator::generate_distance_map(Point finish){
    distance_map = Mat(map.rows, map.cols,CV_32SC1,Scalar(-1));
    // cout<<distance_map.cols<<distance_map.rows<<endl;
     // Color fill map image
    queue<Point> mq;
    mq.push(Point(5,5));
    // cout<<"Generating distances"<<endl;
    while (!mq.empty()){
        Point coord_origen = mq.front();
        mq.pop();
       
        //Append neigbors if valid
        Point north = Point(coord_origen.x, coord_origen.y + 1);
        Point south = Point(coord_origen.x, coord_origen.y - 1);
        Point east = Point(coord_origen.x + 1, coord_origen.y);
        Point west = Point(coord_origen.x - 1, coord_origen.y);
        int value_of_current_point = distance_map.at<int>(coord_origen);
        if(value_of_current_point==-1)++value_of_current_point;
        // cout<<value_of_current_point<<endl;        
        if (notVisited(north))
        {
            distance_map.at<int>(north) = value_of_current_point+1;
            mq.push(north);
        }
        if (notVisited(south))
        {
            distance_map.at<int>(south) = value_of_current_point+1;
            mq.push(south);
        }
        if (notVisited(east))
        {
            distance_map.at<int>(east) = value_of_current_point+1;
            mq.push(east);
        }
        if (notVisited(west))
        {
            distance_map.at<int>(west) = value_of_current_point+1;
            mq.push(west);
        }
    }
    // cout<<"Done calculating distances!"<<endl;
    
}

bool Navigator::notVisited(Point point){
    if(point.x>0 && point.x<map.cols){
        if(point.x>0 && point.y<map.rows){
            return distance_map.at<int>(point) == -1 && this->map.at<Vec3b>(point)[0]==255;
        }
        return false;
    }
    return false;
}

void Navigator::scanMap()
{
    this->grid_map = this->map.clone();
    Vec3b color_grid(0, 0, 255);
    for(int x = 0; x<30; x++){
        for(int y = 0; y<20; y++){
            if((int)grid_map.at<Vec3b>(Point(10+y*map.cols/20,10+x*map.rows/30))[0] == 255)
            {
                //// cout<<"Point found"<<x<<" "<<y<<endl;
                map_grid.grid[x][y].existent = true;
                map_grid.grid[x][y].passed = false;
                // map_grid.grid[x][y].coord.x = y;
                // map_grid.grid[x][y].coord.y = x;
                grid_map.at<Vec3b>(10+x*map.rows/30, 10+y*map.cols/20)=color_grid;
            }else{
                map_grid.grid[x][y].existent = false;
                map_grid.grid[x][y].passed = true;
            }
        }
    }

    findEntrances();
}

void Navigator::findEntrances(){
    Vec3b color_entrance(255, 0, 0);
    bool found = false;
    for(int x = 4; x>=0 && found==false; x--){
        for(int y = 20-4; y<20; y++){
            if(map_grid.grid[x][y].existent)
            {
                entrances.push_back(Point(x,y));
                circle(grid_map, Point(10+y*map.cols/20, 10+x*map.rows/30), 1, Scalar(255, 255, 0), 2);
                found = true;
                break;
            }
        }
    }
    found = false;
    for(int x = 3; x>=0 && found==false; x--){
        for(int y = 2; y>=0; y--){
            if(map_grid.grid[x][y].existent)
            {
                entrances.push_back(Point(x,y));
                circle(grid_map, Point(10+y*map.cols/20, 10+x*map.rows/30), 1, Scalar(255, 255, 0), 2);
                found = true;
                break;
            }
        }
    }
    found = false;
    for(int x = 30-4; x<30 && found==false; x++){
        for(int y = 2; y>=0; y--){
            if(map_grid.grid[x][y].existent)
            {
                entrances.push_back(Point(x,y));
                circle(grid_map, Point(10+y*map.cols/20, 10+x*map.rows/30), 1, Scalar(255, 255, 0), 2);
                found = true;
                break;
            }
        }
    }
    
    found = false;
    for(int x = 30-4; x<30 && found==false; x++){
        for(int y = 20-3; y<20; y++){
            if(map_grid.grid[x][y].existent)
            {
                entrances.push_back(Point(x,y));
                circle(grid_map, Point(10+y*map.cols/20, 10+x*map.rows/30), 1, Scalar(255, 255, 0), 2);
                found = true;
                break;
            }
        }
    }
    if(entrances.size() < 4)
    {
        // cout<<"Entrances not found";
    }else{
        // cout<<"Entrances found!"<<endl;
        //imshow("Grid", grid_map);
        this->map = grid_map.clone();
    }
}

void Navigator::findPath(int entrance, Point finish, bool right)
{
    //Clean previous path
    this->path.clear();
    for(int i=0;i<30;i++){
        for(int x=0;x<20;x++){
            if(map_grid.grid[i][x].existent)
            map_grid.grid[i][x].passed = false;
        }
    }
    Point explorer;
    Point finish_debug, closest_valid;
    //Mat path_map = this->map.clone();
    //circle(path_map, finish, 1, Scalar(0, 255, 0), 2);
    finish_debug.y = (int)((finish.x-10.0)*20/map.cols);
    finish_debug.x = (int)((finish.y-10.0)*30/map.rows);
    double distance = 3000.0;
    if(!map_grid.grid[finish_debug.x][finish_debug.y].existent){
        for(int x=0; x<30; x++){
            for(int y=0; y<20; y++){
                if(map_grid.grid[x][y].existent){
                    double new_distance = sqrtf(pow(finish_debug.x - x, 2) + pow(finish_debug.y - y, 2));
                    if(new_distance < distance){
                        closest_valid.x=x;
                        closest_valid.y=y;
                        distance = new_distance;
                    }
                }
            }
        }
    }
    if(!map_grid.grid[closest_valid.x][closest_valid.y].existent){
        // cout<<"Finishing node not found"<<endl;
        return; 
    }
    finish_debug = closest_valid;
    //circle(path_map, Point(10+finish_debug.y*map.cols/20, 10+finish_debug.x*map.rows/30), 1, Scalar(255, 0, 0), 2);
    //Se usan las coordenadas [indices] de los nodos
    if(entrances.size()==0){
        // cout<<"Cant find path without a valid entrance"<<endl;
        return;
    }
    explorer = entrances[entrance];
    bool reached = false;
    bool changed = false;
    map_grid.grid[explorer.x][explorer.y].passed = true;
    bool go_down = explorer.x < finish_debug.x;
    bool go_left = explorer.y > finish_debug.y;
    array<int,5> priority_order;

    priority_order[4]=5;
    if(right){
        priority_order[0]=(go_down)?2:1;
        priority_order[1]=(!go_down)?2:1;
        priority_order[2]=(go_left)?3:4;
        priority_order[3]=(!go_left)?3:4;
    }else{
        priority_order[0]=(go_left)?3:4;
        priority_order[1]=(!go_left)?3:4;
        priority_order[2]=(go_down)?2:1;
        priority_order[3]=(!go_down)?2:1;
    }

    // for(int i=0; i<5; i++){
    //     // cout<<priority_order[i]<<" ";
    // }
    // // cout<<endl;

    path.push_back(Point(10+(explorer.y)*map.cols/20, 10+(explorer.x)*map.rows/30));
    while(!reached)
    {
        changed=false;
        for(int x : priority_order){
            switch (x)
            {
            case 1:
                while(explorer.x < finish_debug.x && !map_grid.grid[explorer.x+1][explorer.y].passed)
                {    //go down
                    // cout<<"Go down"<<endl;
                    explorer.x++;
                    changed = true;
                    path.push_back(Point(10+(explorer.y)*map.cols/20, 10+(explorer.x)*map.rows/30));
                    map_grid.grid[explorer.x][explorer.y].passed = true;
                }
                break;
            case 2:
                while(explorer.x > finish_debug.x && !map_grid.grid[explorer.x-1][explorer.y].passed)
                {    //go up
                    // cout<<"Go up"<<endl;
                    explorer.x--;
                    changed = true;
                    path.push_back(Point(10+(explorer.y)*map.cols/20, 10+(explorer.x)*map.rows/30));
                    map_grid.grid[explorer.x][explorer.y].passed = true;
                }
                break;
            case 3:
                while(explorer.y > finish_debug.y && !map_grid.grid[explorer.x][explorer.y-1].passed)
                {   //got left
                    // cout<<"Go left"<<endl;
                    explorer.y--;
                    changed = true;
                    path.push_back(Point(10+(explorer.y)*map.cols/20, 10+(explorer.x)*map.rows/30));
                    map_grid.grid[explorer.x][explorer.y].passed = true;
                }
                break;
            case 4:
                while(explorer.y < finish_debug.y && !map_grid.grid[explorer.x][explorer.y+1].passed)
                {   //got right
                    // cout<<"Go right"<<endl;
                    explorer.y++;
                    changed = true;
                    path.push_back(Point(10+(explorer.y)*map.cols/20, 10+(explorer.x)*map.rows/30));
                    map_grid.grid[explorer.x][explorer.y].passed = true;
                }
                break;
            ////////////////////////////////////////////////////////////////////////////////
            case 5:
                if(!changed){
                    for(int x=0; x<priority_order.size()-1 && !changed; x++){
                        switch (priority_order[x])
                        {
                        case 1:
                            if(!map_grid.grid[explorer.x+1][explorer.y].passed)
                            {    //go down
                                // cout<<"Go down !"<<endl;
                                explorer.x++;
                                changed = true;
                                path.push_back(Point(10+(explorer.y)*map.cols/20, 10+(explorer.x)*map.rows/30));
                                map_grid.grid[explorer.x][explorer.y].passed = true;
                            }
                            break;
                        case 2:
                            if(!map_grid.grid[explorer.x-1][explorer.y].passed)
                            {    //go up
                                // cout<<"Go up !"<<endl;
                                explorer.x--;
                                changed = true;
                                path.push_back(Point(10+(explorer.y)*map.cols/20, 10+(explorer.x)*map.rows/30));
                                map_grid.grid[explorer.x][explorer.y].passed = true;
                            }
                            break;
                        case 3:
                            if(!map_grid.grid[explorer.x][explorer.y-1].passed)
                            {   //got left
                                // cout<<"Go left !"<<endl;
                                explorer.y--;
                                changed = true;
                                path.push_back(Point(10+(explorer.y)*map.cols/20, 10+(explorer.x)*map.rows/30));
                                map_grid.grid[explorer.x][explorer.y].passed = true;
                            }
                            break;
                        case 4:
                            if(!map_grid.grid[explorer.x][explorer.y+1].passed)
                            {   //got right
                                // cout<<"Go right !"<<endl;
                                explorer.y++;
                                changed = true;
                                path.push_back(Point(10+(explorer.y)*map.cols/20, 10+(explorer.x)*map.rows/30));
                                map_grid.grid[explorer.x][explorer.y].passed = true;
                            }
                        }
                    }
                }
                break;
            }
        }
        if(explorer.x == finish_debug.x && explorer.y == finish_debug.y){
            //// cout<<"Reached"<<endl;
            reached = true;
        }
    }
    // cout<<"Number of steps: "<<this->path.size()<<endl;
    // Point previous = path[0];
    // for(int i=1; i<path.size(); i++){
    //     line(path_map, previous, path[i], Scalar(255, 0, 255), 2);
    //     previous = path[i];
    // }

    // imshow("Path",path_map);
    // waitKey(0);
    // destroyWindow("Path");
}

vector<Point> Navigator::getPath(){
    return this->path;
}

void Navigator::findPathTesting(int entrance, Point finish, bool right){
    findPath(entrance, finish, right);
    this->path.clear();
    for(int i=0;i<30;i++){
        for(int x=0;x<20;x++){
            if(map_grid.grid[i][x].existent)
            map_grid.grid[i][x].passed = false;
        }
    }   
}

// void Navigator::startTravel();
// void Navigator::displayCarPosition(Pointrd car_position, bool car_orientation);
// void Navigator::nextStep();