#include "FindParkingSpace.h"

FindParkingSpace::FindParkingSpace(Mat parking_lot_image, string screenName)
{
    this->map = parking_lot_image.clone();
    this->objectAnalysis = ObjectAnalysis(parking_lot_image, screenName);
    this->nav = Navigator(parking_lot_image, screenName);
    this->robot = 10;
}

void FindParkingSpace::showRobotTravel(Mat &map_image)
{
    //get path to destination
    vector<Point> path = this->nav.getPath();
    while (path.size() > 0)
    {
        Point current_position = path.front();
        Mat map_to_display = map_image.clone();
        //paint path
        paint_in_map_to_display(path, map_to_display);
        //paith robot
        paint_in_map_to_display(this->robot, current_position, map_to_display);
        //remove the current_position from the vector
        path.erase(path.begin());
    }
}

void FindParkingSpace::paint_in_map_to_display(vector<Point> path, Mat &map_to_display)
{
    for (int i = 0; i < path.size() - 1; i++)
    {
        line(map_to_display, path[i], path[i + 1], Scalar(255, 255, 255), 1, LINE_8, 0);
    }
}
void FindParkingSpace::paint_in_map_to_display(double robot_radius, Point current_position, Mat &map_to_display)
{
    circle(map_to_display, current_position, robot_radius, Scalar(0, 0, 0), -1, 4);
}