#include "FindParkingSpace.h"

FindParkingSpace::FindParkingSpace(Mat parking_lot_image, string screenName)
{
    //Uncomment below to generate base image
    // this->map = parking_lot_image.clone();
    // generateBaseImages();

    //Click callback
    this->screenName = screenName;
    namedWindow(screenName);
    setMouseCallback(screenName, onMouse, this);
    

    //Initialize objects and Mats
    resize(parking_lot_image, parking_lot_image, cv::Size(), 0.8, 0.8);
    finalPoint = NULL;
    this->map = parking_lot_image.clone();
    this->original = parking_lot_image.clone();
    this->slots = imread("slots2.jpg", CV_LOAD_IMAGE_COLOR);
    resize(this->slots, this->slots, cv::Size(), 0.8, 0.8);
    navigator_map = imread("parking_area.jpg", IMREAD_UNCHANGED);
    resize(navigator_map, navigator_map, cv::Size(), 0.8, 0.8);
    this->nav = Navigator(navigator_map, screenName);
    this->robot = 10;
}

void FindParkingSpace::findPath(int entrance, bool direction)
{
    map_helper = this->original.clone();
    if(finalPoint!=NULL){
        cout<<entrance<<" "<<direction<<endl;
        this->nav.findPath(entrance, *finalPoint, direction);
        vector<Point> path = this->nav.getPath();
        if (path.size() > 0)
        {
            //paint path
            cout<<"Path found"<<endl;
            path.push_back(*finalPoint);
            colorFillSlot(map_helper);
            paint_in_map_to_display(path, map_helper);
        }else{
            cout<<"Path not found"<<endl;
        }
    }
    imshow(screenName, map_helper);
    moveWindow(screenName, 0, 0);
}

void FindParkingSpace::showRobotTravel()
{
    cout << "Showing robot travel" << endl;
    //get path to destination
    vector<Point> path = this->nav.getPath();

    //dummy path
    //vector<Point> path = {Point(855, 750), Point(897, 530),Point(897, 411), Point(804, 415)};
    if(path.size() == 0) return;
    path.push_back(*finalPoint);
    while (path.size() > 0)
    {
        cout << path.size() << endl;
        Point current_position = path.front();
        Mat map_to_display = this->original.clone();
        //paint path
        colorFillSlot(map_to_display);
        //paint path
        paint_in_map_to_display(path, map_to_display);
        //paith robot
        paint_in_map_to_display(this->robot, current_position, map_to_display);
        //remove the current_position from the vector
        imshow(screenName, map_to_display);
        waitKey(0);
        path.erase(path.begin());
    }
}

std::vector<std::vector<cv::Point>> FindParkingSpace::contourApproximation(std::vector<std::vector<cv::Point>> contours){
  int min_x, min_y, max_x, max_y;
  std::vector<std::vector<cv::Point>> approx;

  for(auto &vec: contours){
    std::vector<cv::Point> tmp;
    min_x = map.cols;
    min_y = map.rows;
    max_x = max_y = 0;

    for(cv::Point &p : vec){
      min_x = std::min(min_x, p.x);
      max_x = std::max(max_x, p.x);
      min_y = std::min(min_y, p.y);
      max_y = std::max(max_y, p.y);
    }

    tmp.push_back(cv::Point(min_x, min_y));
    tmp.push_back(cv::Point(max_x, min_y));
    tmp.push_back(cv::Point(max_x, max_y));
    tmp.push_back(cv::Point(min_x, max_y));
    approx.push_back(tmp);
  }

  return approx;
}

void FindParkingSpace::generateBaseImages(){
    // Convert to gray scale
    cv::Mat grayImg;
    cv::cvtColor(map, grayImg, CV_BGR2GRAY);

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
    int dilation_type = cv::MORPH_RECT, dilation_size = 2, dilation_size2 = 6;
    cv::Mat element = cv::getStructuringElement(dilation_type,cv::Size( 2*dilation_size + 1, 2*dilation_size+1 ),cv::Point( dilation_size, dilation_size ));
    cv::Mat element2 = cv::getStructuringElement(dilation_type,cv::Size( 2*dilation_size2 + 1, 2*dilation_size2+1 ),cv::Point( dilation_size2, dilation_size2 ));
    erode(result, result, element);
    erode(result, result, element2);
    dilate(result, result, element2);
    medianBlur(result, result, 5);
    
    // Find and sort contours
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(result.clone(), contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0,0));
    std::sort(contours.begin(), contours.end(), compareContourAreas);

    // Create empty matrix for drawing
    cv::Mat drawing = cv::Mat::zeros(map.size(), map.type());
    
    // Draw parking area
    cv::drawContours(drawing, contours, contours.size()-1, cv::Scalar(255,255,255), -1, 8, hierarchy, 0, cv::Point(0,0)); 
    contours.pop_back();

    // Delete three middle car zones from parking area
    for(int i = 0; i < 3; i++){
        cv::drawContours(drawing, contours, contours.size()-1, cv::Scalar(0,0,0), -1, 8, hierarchy, 0, cv::Point(0,0));
        contours.pop_back();
    }

    // Write parking area image
    dilate(drawing, drawing, element2);
    dilate(drawing, drawing, element2);
    erode(drawing, drawing, element);
    erode(drawing, drawing, element2);
    erode(drawing, drawing, element2);
    erode(drawing, drawing, element2);
    erode(drawing, drawing, element2);
    cv::imwrite("parking_area.jpg", drawing);

    // Draw parking slots
    std::vector<std::vector<cv::Point>> approxContours = contourApproximation(contours);
    drawing = cv::Mat::zeros(map.size(), map.type());
    for(int i = 0; i < contours.size(); i++){
        double area = cv::contourArea(contours[i]);
        if(area > 1250){
            // Uncomment the following 3 lines to save rectangled shapes
            cv::Point startP(approxContours[i][0].x, approxContours[i][0].y);
            cv::Point finalP(approxContours[i][2].x, approxContours[i][2].y);
            cv::rectangle(drawing, startP, finalP, cv::Scalar(255,255,255), CV_FILLED);
            // Uncomment thie following line to save raw shapes
            //cv::drawContours(drawing, contours, i, cv::Scalar(255,255,255), -1, 8, hierarchy, 0, cv::Point(0,0)); 
        }
    }
    // Write car slots image
    cv::imwrite("slots2.jpg", drawing);
}

void FindParkingSpace::validateFinalPoint(Point p){
    if(this->slots.at<Vec3b>(p.y, p.x)[0] == 255){
        free(finalPoint);
        finalPoint = (Point*) malloc(sizeof(Point));
        (*finalPoint).x = p.x;
        (*finalPoint).y = p.y;
    }
}

void FindParkingSpace::colorFillSlot(Mat &map_to_display){
    // Fix pixels
    cv::cvtColor(this->slots, this->slots, CV_BGR2GRAY);
    cv::threshold(this->slots,this->slots,127,255,CV_THRESH_BINARY);
    cv::cvtColor(this->slots, this->slots, CV_GRAY2BGR);
    // Color fill map image
    this->slots = imread("slots2.jpg", CV_LOAD_IMAGE_COLOR);
    resize(this->slots, this->slots, cv::Size(), 0.8, 0.8);
    queue<Point> mq;
    mq.push(*finalPoint);
    while (!mq.empty()){
        Point coord_origen = mq.front();
        mq.pop();
        //Append neigbors if valid
        Point north = Point(coord_origen.x, coord_origen.y + 1);
        Point south = Point(coord_origen.x, coord_origen.y - 1);
        Point east = Point(coord_origen.x + 1, coord_origen.y);
        Point west = Point(coord_origen.x - 1, coord_origen.y);
        if (slots.at<Vec3b>(north)[0] == 255)
        {
            slots.at<Vec3b>(north) = Vec3b(0,0,0);
            map_to_display.at<Vec3b>(north) = Vec3b(0,0,255);
            mq.push(north);
        }
        if (slots.at<Vec3b>(south)[0] == 255)
        {
            slots.at<Vec3b>(south) = Vec3b(0,0,0);
            map_to_display.at<Vec3b>(south) = Vec3b(0,0,255);
            mq.push(south);
        }
        if (slots.at<Vec3b>(east)[0] == 255)
        {
            slots.at<Vec3b>(east) = Vec3b(0,0,0);
            map_to_display.at<Vec3b>(east) = Vec3b(0,0,255);
            mq.push(east);
        }
        if (slots.at<Vec3b>(west)[0] == 255)
        {
            slots.at<Vec3b>(west) = Vec3b(0,0,0);
            map_to_display.at<Vec3b>(west) = Vec3b(0,0,255);
            mq.push(west);
        }
    }
}

void FindParkingSpace::paint_in_map_to_display(vector<Point> path, Mat &map_to_display)
{
    for (int i = 0; i < path.size() - 1; i++)
    {
        try
        {
            line(map_to_display, path[i], path[i + 1], Scalar(0, 255, 0), 2, LINE_8, 0);
        }
        catch (...)
        {
            cout << "A point in the path is outside the image!";
        }
    }
}

void FindParkingSpace::paint_in_map_to_display(double robot_radius, Point current_position, Mat &map_to_display)
{
    circle(map_to_display, current_position, robot_radius, Scalar(255, 0, 0),CV_FILLED);
}

void FindParkingSpace::onMouse(int event, int x, int y, int, void *userdata)
{
    FindParkingSpace *objectAnalysis = reinterpret_cast<FindParkingSpace *>(userdata);
    objectAnalysis->onMouse(event, x, y);
}

void FindParkingSpace::onMouse(int event, int x, int y)
{
    if(event == CV_EVENT_LBUTTONDOWN){
        validateFinalPoint(Point(x,y));
    }
}

bool FindParkingSpace::compareContourAreas ( std::vector<cv::Point> contour1, std::vector<cv::Point> contour2 ) {
    double i = fabs( contourArea(cv::Mat(contour1)) );
    double j = fabs( contourArea(cv::Mat(contour2)) );
    return ( i < j );
}

void FindParkingSpace::testPathAlgorithm()
{
    for(int i = 0; i<2; i++){
        for(int x=0;x<4;x++){
            for(int y=0; y<10; y++){
                cout<<"Entrance "<<x<<endl;
                finalPoint=NULL;
                namedWindow(screenName);
                setMouseCallback(screenName, onMouse, this);
                while(finalPoint==NULL){
                    imshow(screenName, map);
                    waitKey(1);    
                }
                destroyWindow(screenName);
                this->nav.findPathTesting(x,*finalPoint, i);
            }   
        }
    }
}