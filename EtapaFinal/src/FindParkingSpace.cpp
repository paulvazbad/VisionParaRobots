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

void FindParkingSpace::generateBaseImages(){
    // Convert to gray scale
    cv::Mat grayImg;
    cv::cvtColor((*original), grayImg, CV_BGR2GRAY);

    // Blur image
    cv::Mat blur;
    GaussianBlur(grayImg, blur, cv::Size(3, 3), 0, 0);
    medianBlur(blur, blur, 5);

    // Enhance borders
    cv::Mat sharpened;
    cv::addWeighted(grayImg, 3, blur, -2, 0, sharpened);

    // Band Pass filter -> set all values to max value
    cv::Mat result;
    cv::threshold(sharpened,result,48,255,CV_THRESH_TOZERO); // 48 -> lower bound
    cv::threshold(result,result,194,255,CV_THRESH_TOZERO_INV); // 194 -> upper bound
    cv::threshold(result,result,1,255,CV_THRESH_BINARY);

    // Clean image
    int dilation_type = cv::MORPH_RECT, dilation_size = 2;
    cv::Mat element = cv::getStructuringElement(dilation_type,cv::Size( 2*dilation_size + 1, 2*dilation_size+1 ),cv::Point( dilation_size, dilation_size ));
    erode(result, result, element);
    medianBlur(result, result, 5);
    
    // Find and sort contours
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(result.clone(), contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0,0));
    std::sort(contours.begin(), contours.end(), compareContourAreas);

    // Create empty matrix for drawing
    cv::Mat drawing = cv::Mat::zeros((*original).size(), (*original).type());
    
    // Draw parking area
    cv::drawContours(drawing, contours, contours.size()-1, cv::Scalar(255,255,255), -1, 8, hierarchy, 0, cv::Point(0,0)); 
    contours.pop_back();

    // Delete three middle car zones from parking area
    for(int i = 0; i < 3; i++){
        cv::drawContours(drawing, contours, contours.size()-1, cv::Scalar(0,0,0), -1, 8, hierarchy, 0, cv::Point(0,0));
        contours.pop_back();
    }

    // Write parking area image
    cv::imwrite("parking_area.jpg", drawing);

    // Draw parking slots
    drawing = cv::Mat::zeros((*original).size(), (*original).type());
    for(int i = 0; i < contours.size(); i++){
        double area = cv::contourArea(contours[i]);
        if(area > 1500)
            cv::drawContours(drawing, contours, i, cv::Scalar(255,255,255), -1, 8, hierarchy, 0, cv::Point(0,0)); 
    }

    // Write car slots image
    cv::imwrite("slots.jpg", drawing);
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