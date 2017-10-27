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



// UKF

#define UKF_STATE_DIM 136
#define UKF_MEASUREMENT_DIM 68
#define UKF_SINGLE_PRECISION 1

#ifdef __cplusplus
extern "C" {
#endif

enum ukf_precision_t {
    UKF_PRECISION_FLOAT = 0,
    UKF_PRECISION_DOUBLE = 1
};

uint32_t ukf_config_get_state_dim(void);
uint32_t ukf_config_get_measurement_dim(void);
uint32_t ukf_config_get_control_dim(void);
enum ukf_precision_t ukf_config_get_precision(void);

#ifdef __cplusplus
}
#endif



#endif

