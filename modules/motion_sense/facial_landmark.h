/* facial_landmark.h */
#ifndef FACIAL_LANDMARK_H
#define FACIAL_LANDMARK_H

#include "reference.h"
#include "resource.h"

#include <opencv2/opencv.hpp>

class FacialLandmark : public Reference {
    GDCLASS(FacialLandmark,Reference);
private:
    cv::VideoCapture _vc;


protected:
    static void _bind_methods();

public:

    FacialLandmark();
};


#endif

