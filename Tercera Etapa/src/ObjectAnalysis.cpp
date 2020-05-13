#include "ObjectAnalysis.h"

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
}

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

void ObjectAnalysis::findRegions(int number_of_objects)
{
    //cout << "Finding regions in image..." << endl;
    time_t last_time, curr_time, start_time;
    double seconds;
    last_time = time(NULL);
    start_time = last_time;
    cout << "----------IMAGE: " << IMAGE_WIDTH << "x" << IMAGE_HEIGHT << endl;
    for (int i = 0; i < number_of_objects; i++)
    {
        Coord seed = generateSeed();
        if (is_object_coord(seed))
        {
            queue<Coord> mq;
            mq.push(seed);
            InformationOfRegionFound informationOfRegionFound = grow_region_found(mq);
            cout << "TOTAL REGION AREA" << informationOfRegionFound.size << endl;
            print_ordinary_moments(informationOfRegionFound);
        }
    }
    seconds = difftime(time(NULL), start_time);
    cout << "EXECUTION TIME: " << seconds << endl;
    imshow(screenName, color_image);
    imwrite("./result.jpg", color_image);
}

InformationOfRegionFound ObjectAnalysis::grow_region_found(queue<Coord> &mq)
{
    InformationOfRegionFound informationOfRegionFound;
    informationOfRegionFound.size = 0;
    vector<vector<int>> ordinary_moments(MAX_ORDINARY_MOMENT_P+1, vector<int>(MAX_ORDINARY_MOMENT_Q+1, 0));
    informationOfRegionFound.ordinary_moments  = ordinary_moments;
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
    return informationOfRegionFound;
}
void ObjectAnalysis::add_to_ordinary_moments(InformationOfRegionFound &informationOfRegionFound, Coord origen)
{

    for (int p = 0; p <= MAX_ORDINARY_MOMENT_P; p++)
    {
        for (int q = 0; q <= MAX_ORDINARY_MOMENT_Q; q++)
        {
            informationOfRegionFound.ordinary_moments[p][q] += pow(origen.x, p) * pow(origen.y, q);
        }
    }
}
void ObjectAnalysis::print_ordinary_moments(InformationOfRegionFound informationOfRegionFound){
    for (int p = 0; p <= MAX_ORDINARY_MOMENT_P; p++)
    {
        for (int q = 0; q <= MAX_ORDINARY_MOMENT_Q; q++)
        {
            cout<<"Moment "<<p<<" "<<q<<" :"<<informationOfRegionFound.ordinary_moments[p][q]<<endl;
        }
    }
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