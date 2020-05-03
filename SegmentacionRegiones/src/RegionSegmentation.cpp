#include "RegionSegmentation.h"

RegionSegmentation::RegionSegmentation(Mat image, string screenName)
{

    this->frame = &image;
    this->screenName = screenName;
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

void RegionSegmentation::findRegions(int number_of_objects)
{
    cout << "Finding regions in image..." << endl;
    for (int i = 0; i < number_of_objects; i++)
    {
        cout << "Current try: " << i << endl;
        Coord seed = generateSeed();
        if (is_object_pixel(seed))
        {
            //Random Color for this region
            cout<<"Valid seed found!"<<endl;
            Vec3b color;
            color[0] = 150;
            color[1] = 200;
            color[2] = 100;
            //Initialize Size of region
            int size_of_region = 0;
            LinkedList.push_back(seed);
            Coord coord_origen;
            while (!LinkedList.empty())
            {
                coord_origen = LinkedList.front();
                LinkedList.pop_front();
                //cout << "new coord : " <<coord_origen.x<<": "<<coord_origen.y<< endl;
                Vec3b color_current(0, 0, 255);
                //color_image.at<Vec3b>(coord_origen.y, coord_origen.x) = color_current;
                size_of_region++;
                //imshow(screenName, color_image);
               //waitKey(1);
                //Append neigbors if valid
                Coord north(coord_origen.x, coord_origen.y + 1);
                Coord south(coord_origen.x, coord_origen.y - 1);
                Coord east(coord_origen.x + 1, coord_origen.y);
                Coord west(coord_origen.x - 1, coord_origen.y + 1);
                Coord neighbors[4] = {north, south, east, west};
                //cout << "Testing neighbors" << endl;
                for (Coord neighbor : neighbors)
                {

                    if (is_object_pixel(neighbor))
                    {
                        LinkedList.push_back(neighbor);
                    }
                }
                color_image.at<Vec3b>(coord_origen.y, coord_origen.x) = color;
            }
            cout << "Size of region: " << size_of_region << endl;
        }

        //GrowRegion
    }
    imshow(screenName, color_image);
}

Coord RegionSegmentation::generateSeed()
{
    int x, y;
    x = rand() % IMAGE_WIDTH; // v2 in the range 1 to 100
    y = rand() % IMAGE_HEIGHT;
    return Coord(x, y);
}

bool RegionSegmentation::is_object_pixel(Coord pixel)
{

    if (0 <= pixel.x && pixel.x < IMAGE_WIDTH && 0 <= pixel.y && pixel.y < IMAGE_HEIGHT)
    {
        //cout << "x: " << pixel.x << " y: " << pixel.y << endl;
        //cout << (double)color_image.at<Vec3b>(pixel.y, pixel.x)[0] << endl;
        return ((double)color_image.at<Vec3b>(pixel.y, pixel.x)[0] == 255);
    }
    return false;
}