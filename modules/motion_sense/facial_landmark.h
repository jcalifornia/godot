/* facial_landmark.h */
#ifndef FACIAL_LANDMARK_H
#define FACIAL_LANDMARK_H

#include "reference.h"

class FacialLandmark : public Reference {
    GDCLASS(FacialLandmark,Reference);


protected:
    static void _bind_methods();

public:

    FacialLandmark();
};

#endif

