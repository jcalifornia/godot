/* facial_landmark.cpp */

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/opencv.h>
#include <vector>
#include "facial_landmark.h"
#include "utils.h"
#include "variant.h"



void FacialLandmark::_bind_methods() {
    ClassDB::bind_method(D_METHOD("start"), &FacialLandmark::startStreaming);
    ClassDB::bind_method(D_METHOD("stop"), &FacialLandmark::stopStreaming);

    ADD_SIGNAL(MethodInfo("facial_detect", PropertyInfo(Variant::INT, "rectange"), PropertyInfo(Variant::INT, "points")));
    
    BIND_ENUM_CONSTANT(STATUS_ERROR);
    BIND_ENUM_CONSTANT(STATUS_NONE);
    BIND_ENUM_CONSTANT(STATUS_STOPPED);
    BIND_ENUM_CONSTANT(STATUS_RUNNING);
    
}

FacialLandmark::FacialLandmark() : _vc(0), _state(STATUS_NONE)  {
    
}

void FacialLandmark::startStreaming(){
    if(!_vc.isOpened()){
        ERR_PRINT("FacialLandmark: unable to connect to camera\n");
        _state == STATUS_ERROR;
    }
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    while( _state == STATUS_STOPPED ){
        cv::Mat temp;
        if (!_vc.read(temp)){
            ERR_PRINT("FacialLandmark: unable to read to camera\n");
            break;
        }
        dlib::cv_image<dlib::bgr_pixel> cimg(temp);
        std::vector<dlib::rectangle> faces = detector(cimg);
        for (unsigned long i = 0; i < faces.size(); ++i)
           dlib::full_object_detection dobj = (_model->get_data())(cimg, faces[i]);
        
    }
    
}
void FacialLandmark::stopStreaming(){
    if( _state == STATUS_RUNNING ){
        _state = STATUS_STOPPED;
    }
}


