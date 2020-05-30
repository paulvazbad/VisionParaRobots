#include "FindParkingSpace.h"

FindParkingSpace::FindParkingSpace(Mat parking_lot_image, string screenName)
{
    this->map = parking_lot_image.clone();
    this->objectAnalysis = ObjectAnalysis(parking_lot_image, screenName);
    //this->nav = Navigator(parking_lot_image, screenName);
    this->robot = 10;
    this->showRobotTravel(parking_lot_image);
}

void FindParkingSpace::showRobotTravel(Mat &map_image)
{
    cout << "Showing robot travel" << endl;
    //get path to destination
    //vector<Point> path = this->nav.getPath();

    //dummy path
    vector<Point> path = {Point(855, 750), Point(897, 530),Point(897, 411), Point(804, 415)};
    while (path.size() > 0)
    {
        cout << path.size() << endl;
        Point current_position = path.front();
        Mat map_to_display = map_image.clone();
        //paint path
        paint_in_map_to_display(path, map_to_display);
        //paith robot
        paint_in_map_to_display(this->robot, current_position, map_to_display);
        //remove the current_position from the vector
        imshow("Robot Demo", map_to_display);
        for (;;)
        {
            int x = waitKey(30);
            if (x == 'x')
            {
                break;
            }
        }

        path.erase(path.begin());
    }
}

void FindParkingSpace::paint_in_map_to_display(vector<Point> path, Mat &map_to_display)
{
    for (int i = 0; i < path.size() - 1; i++)
    {
        try
        {
            line(map_to_display, path[i], path[i + 1], Scalar(0, 255, 0), 1, LINE_8, 0);
        }
        catch (...)
        {
            cout << "A point in the path is outside the image!";
        }
    }
}
void FindParkingSpace::paint_in_map_to_display(double robot_radius, Point current_position, Mat &map_to_display)
{
    circle(map_to_display, current_position, robot_radius, Scalar(255, 0, 0), -1, 4);
}