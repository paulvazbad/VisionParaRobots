#include "FindParkingSpace.h"

FindParkingSpace::FindParkingSpace(Mat parking_lot_image, string screenName)
{
    this->map = parking_lot_image.clone();
    this->objectAnalysis = ObjectAnalysis(parking_lot_image, screenName);
    this->nav = Navigator(parking_lot_image, screenName);

}

// void FindParkingSpace::findPath(Mat map_image)
// {
    
// }
