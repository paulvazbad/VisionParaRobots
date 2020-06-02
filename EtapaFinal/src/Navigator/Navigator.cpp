#include "Navigator.h"

Navigator::Navigator(Mat map, string screenName)
{
    this-> map = map.clone();
    cout<<"Scanning map"<<endl;
    scanMap();
    cout<<"Finding path"<<endl;
    //findPath(3,Point(0,0),false);
}

void Navigator::scanMap()
{
    Mat binary_map = this->map.clone();
    Vec3b color_grid(0, 0, 255);
    for(int x = 0; x<20; x++){
        for(int y = 0; y<20; y++){
            if((int)binary_map.at<Vec3b>(Point(10+y*map.cols/20,10+x*map.rows/20))[0] == 255)
            {
                //cout<<"Point found"<<x<<" "<<y<<endl;
                map_grid.grid[x][y].existent = true;
                map_grid.grid[x][y].passed = false;
                // map_grid.grid[x][y].coord.x = y;
                // map_grid.grid[x][y].coord.y = x;
                binary_map.at<Vec3b>(10+x*map.rows/20, 10+y*map.cols/20)=color_grid;
            }else{
                map_grid.grid[x][y].existent = false;
                map_grid.grid[x][y].passed = true;
            }
        }
    }

    //Find entrances
    Vec3b color_entrance(255, 0, 0);
    bool found = false;
    for(int x = 2; x>=0 && found==false; x--){
        for(int y = 2; y>=0; y--){
            if(map_grid.grid[x][y].existent)
            {
                entrances.push_back(Point(x,y));
                circle(binary_map, Point(10+y*map.cols/20, 10+x*map.rows/20), 1, Scalar(255, 255, 0), 2);
                found = true;
                break;
            }
        }
    }
    found = false;
    for(int x = 20-2; x<20 && found==false; x++){
        for(int y = 2; y>=0; y--){
            if(map_grid.grid[x][y].existent)
            {
                entrances.push_back(Point(x,y));
                circle(binary_map, Point(10+y*map.cols/20, 10+x*map.rows/20), 1, Scalar(255, 255, 0), 2);
                found = true;
                break;
            }
        }
    }
    found = false;
    for(int x = 2; x>=0 && found==false; x--){
        for(int y = 20-2; y<20; y++){
            if(map_grid.grid[x][y].existent)
            {
                entrances.push_back(Point(x,y));
                circle(binary_map, Point(10+y*map.cols/20, 10+x*map.rows/20), 1, Scalar(255, 255, 0), 2);
                found = true;
                break;
            }
        }
    }
    found = false;
    for(int x = 20-2; x<20 && found==false; x++){
        for(int y = 20-2; y<20; y++){
            if(map_grid.grid[x][y].existent)
            {
                entrances.push_back(Point(x,y));
                circle(binary_map, Point(10+y*map.cols/20, 10+x*map.rows/20), 1, Scalar(255, 255, 0), 2);
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

void Navigator::findPath(int entrance, Point finish, bool right)
{
    Point explorer;
    Point finish_debug, closest_valid;
    circle(this->map, finish, 1, Scalar(0, 255, 0), 2);
    finish_debug.y = (int)((finish.x-10.0)*20/map.cols);
    finish_debug.x = (int)((finish.y-10.0)*20/map.rows);
    double distance = 3000.0;
    if(!map_grid.grid[finish_debug.x][finish_debug.y].existent){
        for(int x=0; x<20; x++){
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
        cout<<"Finishing node not found"<<endl;
        waitKey(0);   
    }
    finish_debug = closest_valid;
    circle(this->map, Point(10+finish_debug.y*map.cols/20, 10+finish_debug.x*map.rows/20), 1, Scalar(255, 0, 0), 2);

    //Se usan las coordenadas [indices] de los nodos
    if(entrances.size()==0){
        cout<<"Cant find path without a valid entrance"<<endl;
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
        priority_order[0]=(go_down)?1:2;
        priority_order[1]=(!go_down)?2:1;
        priority_order[2]=(go_left)?3:4;
        priority_order[3]=(!go_left)?4:3;
        // priority_order = {1,2,3,4,5};
    }else{
        priority_order[0]=(go_left)?3:4;
        priority_order[1]=(!go_left)?4:3;
        priority_order[2]=(go_down)?1:2;
        priority_order[3]=(!go_down)?2:1;
        // priority_order = {3,4,1,2,6};
    }

    while(!reached)
    {
        changed=false;
        for(int x : priority_order){
            switch (x)
            {
            case 1:
                while(explorer.x < finish_debug.x && !map_grid.grid[explorer.x+1][explorer.y].passed)
                {    //go down
                    cout<<"Go down"<<endl;
                    explorer.x++;
                    changed = true;
                    path.push_back(Point(10+(explorer.y)*map.cols/20, 10+(explorer.x)*map.rows/20));
                    map_grid.grid[explorer.x][explorer.y].passed = true;
                }
                break;
            case 2:
                while(explorer.x > finish_debug.x && !map_grid.grid[explorer.x-1][explorer.y].passed)
                {    //go up
                    cout<<"Go up"<<endl;
                    explorer.x--;
                    changed = true;
                    path.push_back(Point(10+(explorer.y)*map.cols/20, 10+(explorer.x)*map.rows/20));
                    map_grid.grid[explorer.x][explorer.y].passed = true;
                }
                break;
            case 3:
                while(explorer.y > finish_debug.y && !map_grid.grid[explorer.x][explorer.y-1].passed)
                {   //got left
                    cout<<"Go left"<<endl;
                    explorer.y--;
                    changed = true;
                    path.push_back(Point(10+(explorer.y)*map.cols/20, 10+(explorer.x)*map.rows/20));
                    map_grid.grid[explorer.x][explorer.y].passed = true;
                }
                break;
            case 4:
                while(explorer.y < finish_debug.y && !map_grid.grid[explorer.x][explorer.y+1].passed)
                {   //got right
                    cout<<"Go right"<<endl;
                    explorer.y++;
                    changed = true;
                    path.push_back(Point(10+(explorer.y)*map.cols/20, 10+(explorer.x)*map.rows/20));
                    map_grid.grid[explorer.x][explorer.y].passed = true;
                }
                break;
            ////////////////////////////////////////////////////////////////////////////////
            case 5:
                if(!changed){
                    for(int x=0; x<priority_order.size()-1 && !changed; x++){
                        switch (x)
                        {
                        case 1:
                            if(!map_grid.grid[explorer.x+1][explorer.y].passed)
                            {    //go down
                                cout<<"Go down !"<<endl;
                                explorer.x++;
                                changed = true;
                                path.push_back(Point(10+(explorer.y)*map.cols/20, 10+(explorer.x)*map.rows/20));
                                map_grid.grid[explorer.x][explorer.y].passed = true;
                            }
                            break;
                        case 2:
                            if(!map_grid.grid[explorer.x-1][explorer.y].passed)
                            {    //go up
                                cout<<"Go up !"<<endl;
                                explorer.x--;
                                changed = true;
                                path.push_back(Point(10+(explorer.y)*map.cols/20, 10+(explorer.x)*map.rows/20));
                                map_grid.grid[explorer.x][explorer.y].passed = true;
                            }
                            break;
                        case 3:
                            if(!map_grid.grid[explorer.x][explorer.y-1].passed)
                            {   //got left
                                cout<<"Go left !"<<endl;
                                explorer.y--;
                                changed = true;
                                path.push_back(Point(10+(explorer.y)*map.cols/20, 10+(explorer.x)*map.rows/20));
                                map_grid.grid[explorer.x][explorer.y].passed = true;
                            }
                            break;
                        case 4:
                            if(!map_grid.grid[explorer.x][explorer.y+1].passed)
                            {   //got right
                                cout<<"Go right !"<<endl;
                                explorer.y++;
                                changed = true;
                                path.push_back(Point(10+(explorer.y)*map.cols/20, 10+(explorer.x)*map.rows/20));
                                map_grid.grid[explorer.x][explorer.y].passed = true;
                            }
                        }
                    }
                }
                break;
            }
        }
        if(explorer.x == finish_debug.x && explorer.y == finish_debug.y){
            cout<<"Reached"<<endl;
            reached = true;
        }

        Point previous = path[0];
        for(int i=1; i<path.size(); i++){
            line(this->map, previous, path[i], Scalar(255, 0, 255), 2);
            previous = path[i];
        }
        imshow("Path",this->map);
        waitKey(0);
    }

    cout<<"Number of steps: "<<this->path.size()<<endl;
    Point previous = path[0];
    for(int i=1; i<path.size(); i++){
        line(this->map, previous, path[i], Scalar(255, 0, 255), 2);
        previous = path[i];
    }
    imshow("Path",this->map);
    waitKey(0);
}

vector<Point> Navigator::getPath(){
    return this->path;
}
// void Navigator::startTravel();
// void Navigator::displayCarPosition(Pointrd car_position, bool car_orientation);
// void Navigator::nextStep();