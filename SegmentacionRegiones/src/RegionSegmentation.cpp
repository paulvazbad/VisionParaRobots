#include "RegionSegmentation.h"

RegionSegmentation::RegionSegmentation(Mat image, string screenName)
{
    
    this->frame = &image;
    this->screenName = screenName;
    grayscaleImage = bgrToGray();
    cv::Mat color_image (frame->size(), CV_8U);
    cvtColor(grayscaleImage,color_image,COLOR_GRAY2RGB);
    IMAGE_HEIGHT = grayscaleImage.rows;
    IMAGE_WIDTH = grayscaleImage.cols;
    imshow(screenName,grayscaleImage);
    printImageInfo(IMAGE_WIDTH/2, IMAGE_HEIGHT/2);
}

Mat RegionSegmentation::bgrToGray()
{
  cv::Mat result (frame->size(), CV_8U);
  Mat binarized_image;
  cvtColor(*frame, result, CV_BGR2GRAY);
  threshold(result,binarized_image,127,255,0);
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
    for (int i = 0; i < number_of_objects;)
    {
        Coord seed = generateSeed();
        if (is_object_pixel(seed))
        {
            i++;
            LinkedList.push_back(seed);
            
        }
        
        //GrowRegion
    }
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
        cout<<"x: "<<pixel.x<<" y: "<<pixel.y<<endl;
        cout<<(double)grayscaleImage.at<Vec3b>(pixel.y, pixel.x)[0]<<endl;
        return ((double)grayscaleImage.at<Vec3b>(pixel.y, pixel.x)[0] == 255);
    }
    return false;
}