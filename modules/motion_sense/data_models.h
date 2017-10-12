#include "resource.h"
#include "reference.h"

#ifndef FACIAL_LANDMARK_MODEL_H
#define FACIAL_LANDMARK_MODEL_H

#include <dlib/image_processing.h>

class FacialLandmarkModel : public Resource{
    GDCLASS(FacialLandmarkModel, Resource);
    
public:
    FacialLandmarkModel();
    Error set_file(const String &p_file);
private:
    dlib::shape_predictor _pose_model;
    String file;
    
};
#endif
