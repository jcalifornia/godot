/* facial_landmark.h */
#ifndef FACIAL_LANDMARK_H
#define FACIAL_LANDMARK_H

#include "reference.h"
#include "resource.h"
#include "data_models.h"
#include <opencv2/opencv.hpp>
#include <dlib/image_processing/frontal_face_detector.h>


#define FACE_DOWNSAMPLE_RATIO 2
#define SKIP_FRAMES 1

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

double variance(std::vector<cv::Point2f> curPoints, std::vector<cv::Point2f> lastPoints);

VARIANT_ENUM_CAST(FacialLandmark::Status);

#endif

