#include "ObjectAnalysis.h"
#if WIN32
#include <windows.h>
#else
#include <X11/Xlib.h>
#endif

ObjectAnalysis::ObjectAnalysis(Mat image, string screenName)
{
    this->frame = &image;
    this->screenName = screenName;
    cout << "HEIGHT " << frame->size().height << endl;
    cout << "WIDTH " << frame->size().width << endl;
    grayscaleImage = bgrToGray();
    this->color_image = grayTobgr(this->color_image);
    IMAGE_HEIGHT = grayscaleImage.rows;
    IMAGE_WIDTH = grayscaleImage.cols;
    printImageInfo(IMAGE_WIDTH / 2, IMAGE_HEIGHT / 2);
    read_model();
    
    namedWindow("HSV Range");
    createTrackbar("H", "HSV Range", &hsvRange[0], 255);
    createTrackbar("S", "HSV Range", &hsvRange[1], 255);
    createTrackbar("V", "HSV Range", &hsvRange[2], 255);

    hsvRange[0] = 8;
    hsvRange[1] = 72;
    hsvRange[2] = 169;

    setTrackbarPos("H", "HSV Range", hsvRange[0]);
    setTrackbarPos("S", "HSV Range", hsvRange[1]);
    setTrackbarPos("V", "HSV Range", hsvRange[2]);

    //Click callback
    namedWindow(screenName);
    setMouseCallback(screenName, onMouse, this);

    //Mira
    //displayResult(None, 0);
}

void ObjectAnalysis::read_model(){
    std::ifstream file("models.txt");
    if(file.fail()){
        std::cout << "Models file cannot be opened\n";
        return;  
    }

    long double ph1, ph2, var1, var2;
    string name;

    getline(file, name);
    while(file >> name >> ph1 >> var1 >> ph2 >> var2){
        ObjectInformation obj = ObjectInformation(name, ph1, ph2);
        obj.set_variance(var1, var2);
        objectModels.push_back(obj);
    }

}

void ObjectAnalysis::printImageInfo(int x, int y)
{
    int KS = 5;
    cout << "Rows: " << grayscaleImage.rows << endl;
    cout << "Cols " << grayscaleImage.cols << endl;
    cout << "Channels: " << grayscaleImage.channels() << endl;

    for (int j = y - (KS / 2); j <= y + (KS / 2); ++j)
    {
        for (int i = x - (KS / 2); i <= x + (KS / 2); ++i)
        {
            if (i > 0 && i < grayscaleImage.cols && j > 0 && j < grayscaleImage.rows)
            {
                double value = grayscaleImage.at<Vec3b>(i, j)[0];

                cout << value << " ";
            }
            else
            {
                cout << "NaN ";
            }
        }
        cout << endl;
    }
}


/////////////////////////////////////////////////////////////////////
//////////////////////   DISPLAY RESULT   ///////////////////////////
/////////////////////////////////////////////////////////////////////

void ObjectAnalysis::displayResult(InformationOfRegionFound inf, int combination){
    int width, height;
    getScreenResolution(width, height);
    Mat mira(height, width,CV_8UC3,Scalar(255,255,255));
    int qx, qy, x, y;

    switch(combination){
        case 0:
            qx = width;
            qy = height/2;
            x = width/2;
            y = 0;
            break;
        case 1:
            qx = width/2;
            qy = height/2;
            x = 0;
            y = 0;
            break;  
        case 2:
            qx = width/2;
            qy = height;
            x = 0;
            y = height/2;
            break;
        case 3:
            qx = width;
            qy = height;
            x = width/2;
            y = height/2;
            break;
    }

    //Draw mira
    rectangle(mira,Point(x,y),Point(qx, qy), Scalar(255, 107, 0), FILLED, LINE_8);
    line(mira, Point(width/2, 0), Point(width/2, height), Scalar(0,0,0), 3);
    line(mira, Point(0, height/2), Point( width, height/2), Scalar(0,0,0), 3);

    //TODO draw pendiente de figura

    imshow("Mira", mira);
    moveWindow("Mira", 0, 0);
    
}

/////////////////////////////////////////////////////////////////////
////////////   IMAGE FILTERING //////////////////////////////////////
/////////////////////////////////////////////////////////////////////

Mat ObjectAnalysis::grayTobgr(Mat color_image)
{
    cv::Mat empty(frame->size(), CV_8U);
    color_image = empty;
    cvtColor(grayscaleImage, color_image, COLOR_GRAY2RGB);
    return color_image;
}

Mat ObjectAnalysis::bgrToGray()
{
    cv::Mat result(frame->size(), CV_8U);
    Mat binarized_image;
    cvtColor(*frame, result, CV_BGR2GRAY);
    threshold(result, binarized_image, 127, 255, 0);
    return binarized_image;
}

void ObjectAnalysis::filterImage(Mat image)
{
    this->frame = &image;
    imshow(screenName, *frame);
    cvtColor(*frame, hsvImage, CV_BGR2HSV);
    hsvFilter();
    imshow("HSV filtered", filteredImage);
    erotion();
    dilation();
    imshow("Final filtered", filteredImage);
}

void ObjectAnalysis::hsvFilter()
{
    Mat result = Mat::zeros((*frame).size(), (*frame).type()), mask;
    int hMin, hMax, sMin, sMax, vMin, vMax;
    calculateMaxMinChannels(HSV_color, hMin, sMin, vMin, hMax, sMax, vMax);

    // Updates mask values with the corresponding H,S,V limits
    inRange(hsvImage, Scalar(hMin, sMin, vMin), Scalar(hMax, sMax, vMax), filteredImage);
}

void ObjectAnalysis::calculateMaxMinChannels(Vec3b &color, int &hMin, int &sMin, int &vMin, int &hMax, int &sMax, int &vMax)
{
    int epsilon[3];
    memcpy(epsilon, hsvRange, 3 * sizeof *hsvRange);

    hMin = max(color[0] - epsilon[0], 0);
    sMin = max(color[1] - epsilon[1], 0);
    vMin = max(color[2] - epsilon[2], 0);
    hMax = min(color[0] + epsilon[0], 255);
    sMax = min(color[1] + epsilon[1], 255);
    vMax = min(color[2] + epsilon[2], 255);
}

void ObjectAnalysis::dilation(){
    Mat dilation_dst;
    int dilation_type = MORPH_RECT;
    int dilation_size = 5; //here increase for more dilation, reduce for less dilation
    Mat element = getStructuringElement( dilation_type,
                                        Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                        Point( dilation_size, dilation_size ) );

    /// Apply the erosion operation
    dilate( filteredImage, filteredImage, element );
    //   imshow("Original Binarized Dilated", eroded);
    //   imshow("Dilation 5x5", dilation_dst);
}


void ObjectAnalysis::erotion(){
    Mat erosion_dst;
    int erosion_type = MORPH_RECT;
    int erosion_size = 5; //here increase for more erosion, reduce for less erosion
    Mat element = getStructuringElement( erosion_type,
                                        Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                        Point( erosion_size, erosion_size ) );
    /// Apply the erosion operation
    erode( filteredImage, filteredImage, element );
    //   imshow("Original Binarized ",binaryImage);
    //   imshow("Erotion 5x5", erosion_dst);
}

void ObjectAnalysis::onMouse(int event, int x, int y, int, void *userdata)
{
  ObjectAnalysis *objectAnalysis = reinterpret_cast<ObjectAnalysis *>(userdata);
  objectAnalysis->onMouse(event, x, y);
}

void ObjectAnalysis::onMouse(int event, int x, int y)
{
  switch (event)
  {
  case CV_EVENT_LBUTTONDOWN:
    cout << "  Mouse X, Y: " << x << ", " << y << endl;
    HSV_color = (hsvImage).at<Vec3b>(Point(x, y));
    cout << "H = " << (int)HSV_color[0] << endl;
    cout << "S = " << (int)HSV_color[1] << endl;
    cout << "V = " << (int)HSV_color[2] << endl;
    break;
  case CV_EVENT_MOUSEMOVE:
    break;
  case CV_EVENT_LBUTTONUP:
    break;
  }
}


//////////////////////////////////////////////////////////////////////



void ObjectAnalysis::save_partial_results(time_t &last_time, time_t &curr_time, double seconds, int number_of_seed)
{
    int target_time = 3;
    curr_time = time(NULL);
    seconds = difftime(curr_time, last_time);
    if (seconds >= target_time)
    {
        cout << "Saving checkpoint!" << endl;
        imwrite("./partial_results/result" + to_string(number_of_seed) + "_" + to_string(target_time) + ".jpg", color_image);
        last_time = curr_time;
        target_time = target_time * 3;
    }
}

void ObjectAnalysis::findRegions(const int number_of_objects,const int SEED_LIMIT)
{
    //cout << "Finding regions in image..." << endl;
    time_t last_time, curr_time, start_time;
    double seconds;
    last_time = time(NULL);
    start_time = last_time;
    cout << "----------IMAGE: " << IMAGE_WIDTH << "x" << IMAGE_HEIGHT << endl;
    int number_regions_found = 0, seeds_deposited=0;
    while(number_regions_found<number_of_objects && seeds_deposited<SEED_LIMIT)
    {
        Coord seed = generateSeed();
        if (is_object_coord(seed))
        {
            queue<Coord> mq;
            mq.push(seed);
            InformationOfRegionFound informationOfRegionFound = grow_region_found(mq);
            cout << "TOTAL REGION AREA = " << informationOfRegionFound.size << endl;
            print_moments(informationOfRegionFound);
            regionsFound.push_back(informationOfRegionFound);
            number_regions_found++;
        }
        seeds_deposited++;
    }
    seconds = difftime(time(NULL), start_time);
    cout << "EXECUTION TIME: " << seconds << endl;
    // imshow(screenName, color_image);
    // waitKey(0);
    imwrite("./result.jpg", color_image);
}

InformationOfRegionFound ObjectAnalysis::grow_region_found(queue<Coord> &mq)
{
    InformationOfRegionFound informationOfRegionFound;
    informationOfRegionFound.size = 0;
    vector<vector<long>> ordinary_moments(MAX_ORDINARY_MOMENT_P + 1, vector<long>(MAX_ORDINARY_MOMENT_Q + 1, 0));
    informationOfRegionFound.ordinary_moments = ordinary_moments;
    Vec3b color_current(0, 0, 200);
    while (!mq.empty())
    {
        Coord coord_origen = mq.front();
        mq.pop();
        informationOfRegionFound.size++;
        //UNCOMMENT THIS TO WATCH THE PROGRESS
        imshow(screenName, color_image);
        waitKey(1);
        add_to_ordinary_moments(informationOfRegionFound, coord_origen);
        //Append neigbors if valid
        paint_and_append_object_neighbors(coord_origen, color_current, mq);
    }

    calculate_moments(informationOfRegionFound);
    draw_moments(informationOfRegionFound, 100);
    cout << "MATCH SHAPE = " << match_shape(informationOfRegionFound) << endl;

    // imshow(screenName, color_image);
    // waitKey(0);

    return informationOfRegionFound;
}

string ObjectAnalysis::match_shape(InformationOfRegionFound inf){
    int closest = 0;
    for(int i = 1; i < objectModels.size() ; i++){
        if(eucladian_distance(inf.ph1, objectModels[i].median_ph1, inf.ph2, objectModels[i].median_ph2) < eucladian_distance(inf.ph1, objectModels[closest].median_ph1, inf.ph2, objectModels[closest].median_ph2))
            closest = i;
    }

    if( fabs(objectModels[closest].median_ph1 - inf.ph1) <= objectModels[closest].variance_ph1 && fabs(objectModels[closest].median_ph2 - inf.ph2) <= objectModels[closest].variance_ph2)
        return objectModels[closest].name_of_object;

    return "";
}

long double ObjectAnalysis::eucladian_distance(long double x1, long double x2, long double y1, long double y2){
    return sqrtf(pow(x2-x1, 2) + pow(y2 - y1, 2));
}

void ObjectAnalysis::calculate_moments(InformationOfRegionFound &inf)
{
    // Centralized moments
    inf.u20 = inf.ordinary_moments[2][0] - (pow(inf.ordinary_moments[1][0], 2) / inf.ordinary_moments[0][0]);
    inf.u02 = inf.ordinary_moments[0][2] - (pow(inf.ordinary_moments[0][1], 2) / inf.ordinary_moments[0][0]);
    inf.u11 = inf.ordinary_moments[1][1] - (inf.ordinary_moments[0][1] * inf.ordinary_moments[1][0] / inf.ordinary_moments[0][0]);
    // Normalized moments
    inf.n20 = get_normalized_moments(inf, inf.u20, 2, 0);
    inf.n02 = get_normalized_moments(inf, inf.u02, 0, 2);
    inf.n11 = get_normalized_moments(inf, inf.u11, 1, 1);
    // Hu moments
    inf.ph1 = inf.n20 + inf.n02;
    inf.ph2 = pow(inf.n20 - inf.n02, 2) + 4 * pow(inf.n11, 2);

    inf.angle = 0.5 * atan2(2 * inf.u11, inf.u20 - inf.u02);
    inf.cx = inf.ordinary_moments[1][0]/inf.ordinary_moments[0][0];
    inf.cy = inf.ordinary_moments[0][1]/inf.ordinary_moments[0][0];
}

long double ObjectAnalysis::get_normalized_moments(InformationOfRegionFound &inf, long centralizedMoment, int p, int q)
{
    double gamma = ((p + q) / 2) + 1;
    return centralizedMoment / (pow(inf.ordinary_moments[0][0], gamma));
}

void ObjectAnalysis::add_to_ordinary_moments(InformationOfRegionFound &informationOfRegionFound, Coord origen)
{

    for (int p = 0; p <= MAX_ORDINARY_MOMENT_P; p++)
    {
        for (int q = 0; q <= MAX_ORDINARY_MOMENT_Q; q++)
        {
            informationOfRegionFound.ordinary_moments[p][q] += (long)(pow(origen.x, p) * pow(origen.y, q));
        }
    }
}

void ObjectAnalysis::print_moments(InformationOfRegionFound informationOfRegionFound)
{
    // for (int p = 0; p <= MAX_ORDINARY_MOMENT_P; p++)
    // {
    //     for (int q = 0; q <= MAX_ORDINARY_MOMENT_Q; q++)
    //     {
    //         cout<<"Moment "<<p<<" "<<q<<": "<<informationOfRegionFound.ordinary_moments[p][q]<<endl;
    //     }
    // }
    // cout << "Moment u20: " << informationOfRegionFound.u20 << endl;
    // cout << "Moment u02: " << informationOfRegionFound.u02 << endl;
    // cout << "Moment u11: " << informationOfRegionFound.u11 << endl;
    // cout << "Moment n20: " << informationOfRegionFound.n20 << endl;
    // cout << "Moment n02: " << informationOfRegionFound.n02 << endl;
    // cout << "Moment n11: " << informationOfRegionFound.n11 << endl;
    cout << "Moment ph1: " << informationOfRegionFound.ph1 << endl;
    cout << "Moment ph2: " << informationOfRegionFound.ph2 << endl;
}

// Hyp determines half the size (in pixels) of the line to pass through
void ObjectAnalysis::draw_moments(InformationOfRegionFound inf, int hyp){
    int adj = cos(inf.angle) * hyp;
    int opp = sin(inf.angle) * hyp;

    circle(color_image, Point(inf.cx,inf.cy), 1, Scalar(0,255,0), 2);
    line(color_image, Point(inf.cx - adj, inf.cy - opp), Point(inf.cx + adj, inf.cy + opp), Scalar(0,255,0), 1);
}

void ObjectAnalysis::paint_and_append_object_neighbors(Coord coord_origen, Vec3b color, queue<Coord> &mq)
{
    Coord north(coord_origen.x, coord_origen.y + 1);
    Coord south(coord_origen.x, coord_origen.y - 1);
    Coord east(coord_origen.x + 1, coord_origen.y);
    Coord west(coord_origen.x - 1, coord_origen.y);
    Coord neighbors[4] = {north, south, east, west};
    for (Coord neighbor : neighbors)
    {
        if (is_object_coord(neighbor))
        {
            this->color_image.at<Vec3b>(neighbor.y, neighbor.x) = color;
            mq.push(neighbor);
        }
    }
}

Coord ObjectAnalysis::generateSeed()
{
    int x, y;
    x = rand() % IMAGE_WIDTH; // v2 in the range 1 to 100
    y = rand() % IMAGE_HEIGHT;
    return Coord(x, y);
}

bool ObjectAnalysis::is_object_coord(Coord coord)
{

    if (0 <= coord.x && coord.x < IMAGE_WIDTH && 0 <= coord.y && coord.y < IMAGE_HEIGHT)
    {
        //
        if ((int)this->color_image.at<Vec3b>(coord.y, coord.x)[0] == 255)
        {
            return true;
        }
        return false;
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////
//////////////////////////////// TRAINING //////////////////////////////////
////////////////////////////////////////////////////////////////////////////

void ObjectAnalysis::captureTrainData(Mat image){
    string name;
    cout << "Name: "<< endl;
    cin >> name;
    imwrite("./train_data/" + name + ".jpg", image);
    cout<<"Image saved"<<endl;
}

void ObjectAnalysis::train(string name_of_object)
{
    //find regions
    this->findRegions(1,1000);
    cout << "Number of objects found in image: " << regionsFound.size() << endl;
    //Assumes only one object in the image provided
    if (name_of_object.size() == 0)
    {
        cout << "Give me the name of the object found: " << endl;
        cin >> name_of_object;
        cout << endl;
    }
    print_moments(regionsFound[0]);
    string save_confirmation = "Y";
    cout << "Do you want to save this moments to the dataset? [Y/n] " << endl;
    cin >> save_confirmation;
    if (save_confirmation == "Y")
    {
        save_moments_to_dataset(name_of_object);
        recalculate_models();
    }
}

void ObjectAnalysis::save_moments_to_dataset(string name_of_object)
{
    ofstream file;
    cout << "Saving into figures_dataset.txt" << endl;
    file.open("figures_dataset.txt", ofstream::out | ofstream::app);
    file << name_of_object << " " << regionsFound[0].ph1 << " " << regionsFound[0].ph2 << "\n";
    file.close();
    cout << "Done saving! " << endl;
}

void ObjectAnalysis::recalculate_models()
{
    unordered_map<string, ObjectInformation> objects_hashmap;
    load_dataset_into_hashmap(objects_hashmap);
    update_median_variance(objects_hashmap);
    //load each entry from figures_dataset.txt and calculate median and variance
}

void ObjectAnalysis::load_dataset_into_hashmap(unordered_map<string, ObjectInformation> &objects_hashmap)
{
    ifstream dataset_file;
    dataset_file.open("figures_dataset.txt");
    string name_of_object;
    long double ph1, ph2;
    //skip titles line
    getline(dataset_file,name_of_object);
    while (dataset_file >> name_of_object >> ph1 >> ph2)
    {
        if (objects_hashmap.find(name_of_object) == objects_hashmap.end())
        {
            objects_hashmap[name_of_object] = ObjectInformation(name_of_object, ph1, ph2);
        }
        else
        {
            objects_hashmap[name_of_object].name_of_object = name_of_object;
            objects_hashmap[name_of_object].median_ph1 += ph1;
            objects_hashmap[name_of_object].median_ph2 += ph2;
            objects_hashmap[name_of_object].ph1s.push_back(ph1);
            objects_hashmap[name_of_object].ph2s.push_back(ph2);
        }
        objects_hashmap[name_of_object].entries_in_dataset++;
    }
    dataset_file.close();
}

void ObjectAnalysis::update_median_variance(unordered_map<string, ObjectInformation> &objects_hashmap)
{
    ofstream file;
    cout << "Saving recalculation into models.txt" << endl;
    file.open("models.txt", ofstream::out);
    file << "Figure_name median_ph1 variance_ph1 median_ph2 variance_ph2" << endl;
    for (auto i : objects_hashmap)
    {
        i.second.calculate_median();
        i.second.calculate_variance();
        cout << i.first << " "
             << "median " << i.second.median_ph1 << " variance " << i.second.variance_ph1 << endl;
        file << i.first << " " << i.second.median_ph1 << " " << i.second.variance_ph1 << " "
             << i.second.median_ph2 << " " << i.second.variance_ph2 << "\n";
    }
    file.close();
    cout << "Done saving! " << endl;
}


////////////////////////////////////////////////////////////////////////////
//////////////////////////////// DISPLAY //////////////////////////////////
////////////////////////////////////////////////////////////////////////////


void ObjectAnalysis::getScreenResolution(int &width, int &height)
{
#if WIN32
  width = (int)GetSystemMetrics(SM_CXSCREEN);
  height = (int)GetSystemMetrics(SM_CYSCREEN) * 0.9;
#else
  Display *disp = XOpenDisplay(NULL);
  Screen *scrn = DefaultScreenOfDisplay(disp);
  width = scrn->width*0.97;
  height = scrn->height * 0.97;
#endif
}
