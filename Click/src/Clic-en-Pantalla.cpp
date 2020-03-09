/*
 * Title: Window Click Example
 * Class: Vision para Robot
 * Instructor: Dr. Jose Luis Gordillo (http://robvis.mty.itesm.mx/~gordillo/)
 * Code: Manlio Barajas (manlito@gmail.com)
 * Institution: Tec de Monterrey, Campus Monterrey
 * Date: January 28, 2013
 *
 * Description: Shows the most basic interaction with OpenCV HighGui window.
 *
 * This programs uses OpenCV http://www.opencv.org/
 */

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// Here we will store points
vector<Point> points;
Mat currentImage;


/* This is the callback that will only display mouse coordinates */
void mouseCoordinatesExampleCallback(int event, int x, int y, int flags, void* param);

int main(int argc, char *argv[])
{
	/* First, open camera device */
	VideoCapture camera;
    camera.open(0);

	/* Create images where captured and transformed frames are going to be stored */
	
    /* Create main OpenCV window to attach callbacks */
    namedWindow("Image");
    setMouseCallback("Image", mouseCoordinatesExampleCallback);

    while (true)
	{
		/* Obtain a new frame from camera */
		camera >> currentImage;

		if (currentImage.data) 
		{
            /* Draw all points */
            for (int i = 0; i < points.size(); ++i) {
                circle(currentImage, (Point)points[i], 5, Scalar( 255, 0, 255 ), CV_FILLED);
                if(i > 0){
                    line(currentImage, points[i], points[i - 1], CV_RGB(65,255,24), 1, 8, 0);
                }
            }

            /* Show image */
            imshow("Image", currentImage);

			/* If 'x' is pressed, exit program */
			if (waitKey(3) == 'x')
				break;
		}
		else
		{
			cout << "No image data.. " << endl;
		}
	}
}


void mouseCoordinatesExampleCallback(int event, int x, int y, int flags, void* param)
{
    switch (event)
    {
        case CV_EVENT_LBUTTONDOWN:
            cout << "  Mouse X, Y: " << x << ", " << y;
            cout << endl;
            /*  Draw a point */
            points.push_back(Point(x, y));
            break;
        case CV_EVENT_MOUSEMOVE:
            break;
        case CV_EVENT_LBUTTONUP:
            break;
    }
}
