#include "RegionSegmentation.h"
#include <set>
RegionSegmentation::RegionSegmentation(Mat image, string screenName)
{

    this->frame = &image;
    this->screenName = screenName;
    cout << "HEIGHT " << frame->size().height << endl;
    cout << "WIDTH " << frame->size().width << endl;
    grayscaleImage = bgrToGray();
    cv::Mat empty(frame->size(), CV_8U);
    this->color_image = empty;
    cvtColor(grayscaleImage, color_image, COLOR_GRAY2RGB);
    IMAGE_HEIGHT = grayscaleImage.rows;
    IMAGE_WIDTH = grayscaleImage.cols;
    printImageInfo(IMAGE_WIDTH / 2, IMAGE_HEIGHT / 2);
}

Mat RegionSegmentation::bgrToGray()
{
    cv::Mat result(frame->size(), CV_8U);
    Mat binarized_image;
    cvtColor(*frame, result, CV_BGR2GRAY);
    threshold(result, binarized_image, 127, 255, 0);
    return binarized_image;
}
void RegionSegmentation::printImageInfo(int x, int y)
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

void RegionSegmentation::save_partial_results(time_t &last_time, time_t &curr_time, double seconds, int number_of_seed)
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

void RegionSegmentation::findRegions(int number_of_objects)
{
    //cout << "Finding regions in image..." << endl;
    time_t last_time, curr_time, start_time;
    double seconds;
    last_time = time(NULL);
    start_time  = last_time;
    cout<<"----------IMAGE: "<<IMAGE_WIDTH<<"x"<<IMAGE_HEIGHT<<endl;
    for (int i = 0; i < number_of_objects; i++)
    {
        //cout << "Current try: " << i << endl;
        Coord seed = generateSeed();
        if (is_object_coord(seed))
        {
            //Random Color for this region
            //cout << "Valid seed found!" << endl;
            //Initialize Size of region
            int size_of_region = 0;
            queue<Coord> mq;
            mq.push(seed);
            
            while (!mq.empty())
            {
                //save_partial_results(last_time,curr_time,seconds,i);
                Coord coord_origen = mq.front();
                mq.pop();
                Vec3b color_current(0, 0, 200);
                size_of_region++;
                //UNCOMMENT THIS TO WATCH THE PROGRESS

                imshow(screenName, color_image);
                waitKey(1);

                //Append neigbors if valid
                Coord north(coord_origen.x, coord_origen.y + 1);
                Coord south(coord_origen.x, coord_origen.y - 1);
                Coord east(coord_origen.x + 1, coord_origen.y);
                Coord west(coord_origen.x - 1, coord_origen.y);
                if (is_object_coord(north))
                {
                    this->color_image.at<Vec3b>(north.y, north.x) = color_current;
                    mq.push(north);
                }
                if (is_object_coord(south))
                {
                    this->color_image.at<Vec3b>(south.y, south.x) = color_current;
                    mq.push(south);
                }
                if (is_object_coord(east))
                {
                    this->color_image.at<Vec3b>(east.y, east.x) = color_current;
                    mq.push(east);
                }
                if (is_object_coord(west))
                {
                    this->color_image.at<Vec3b>(west.y, west.x) = color_current;
                    mq.push(west);
                }
                //this->color_image.at<Vec3b>(coord_origen.y, coord_origen.x);
            }
            cout << "TOTAL REGION AREA" << size_of_region << endl;
        }

        //GrowRegion
    }
    seconds = difftime(time(NULL), start_time);
    cout<<"EXECUTION TIME: "<<seconds<<endl;
    imshow(screenName, color_image);
    imwrite("./result.jpg", color_image);
}

Coord RegionSegmentation::generateSeed()
{
    int x, y;
    x = rand() % IMAGE_WIDTH; // v2 in the range 1 to 100
    y = rand() % IMAGE_HEIGHT;
    return Coord(x, y);
}

bool RegionSegmentation::is_object_coord(Coord coord)
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