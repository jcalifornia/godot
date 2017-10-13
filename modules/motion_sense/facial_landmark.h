/* facial_landmark.h */
#ifndef FACIAL_LANDMARK_H
#define FACIAL_LANDMARK_H

#include "reference.h"
#include "resource.h"
#include "data_models.h"
#include <opencv2/opencv.hpp>

class FacialLandmark : public Reference {
    GDCLASS(FacialLandmark,Reference);
public:
    enum Status{
        STATUS_NONE,
        STATUS_STOPPED,
        STATUS_RUNNING,
        STATUS_ERROR
    };
private:
    cv::VideoCapture _vc;
    Ref<FacialLandmarkModel> _model;
    Status _state;

protected:
    static void _bind_methods();

public:
    
    FacialLandmark();
    void startStreaming();
    void stopStreaming();
    void set_data(Ref<FacialLandmarkModel> m);
};

VARIANT_ENUM_CAST(FacialLandmark::Status);
#endif

