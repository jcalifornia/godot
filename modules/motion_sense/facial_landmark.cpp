/* facial_landmark.cpp */

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/opencv.h>
#include <vector>
#include "facial_landmark.h"
#include "utils.h"
#include "variant.h"
#include "math/math_2d.h"
#include "print_string.h"
#include "dvector.h"
#include "os/os.h"

void FacialLandmark::_bind_methods() {
    ClassDB::bind_method(D_METHOD("start"), &FacialLandmark::startStreaming);
    ClassDB::bind_method(D_METHOD("stop"), &FacialLandmark::stopStreaming);
    ClassDB::bind_method(D_METHOD("set_data", "trained_model"), &FacialLandmark::set_data);


    ADD_SIGNAL(MethodInfo("facial_detect", PropertyInfo(Variant::RECT2, "bounding_box"), PropertyInfo(Variant::POOL_VECTOR2_ARRAY, "points")));
    
    BIND_ENUM_CONSTANT(STATUS_ERROR);
    BIND_ENUM_CONSTANT(STATUS_NONE);
    BIND_ENUM_CONSTANT(STATUS_STOPPED);
    BIND_ENUM_CONSTANT(STATUS_RUNNING);
    
}

FacialLandmark::FacialLandmark() : _vc(0), _state(STATUS_NONE)  {
    
}

void FacialLandmark::set_data(Ref<FacialLandmarkModel> m){
    _model = m;
}

void FacialLandmark::startStreaming(){
    if(!_vc.isOpened()){
        ERR_PRINT("FacialLandmark: unable to connect to camera\n");
        _state = STATUS_ERROR;
    }
    OS *os = OS::get_singleton();
    _state = STATUS_RUNNING;
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    while( _state == STATUS_RUNNING ){
        cv::Mat temp;
        
        int startTime = os ->get_ticks_msec();
        if (!_vc.read(temp)){
            ERR_PRINT("FacialLandmark: unable to read to camera\n");
            break;
        }
        dlib::cv_image<dlib::bgr_pixel> cimg(temp);
        std::vector<dlib::rectangle> faces = detector(cimg);
        //print_line("delected x faces: " + itos(faces.size()));
        
        PoolVector<Vector2> pts;
        if(faces.size()>0) {
            dlib::full_object_detection dobj = (_model->get_data())(cimg, faces[0]);
            pts.resize(dobj.num_parts());
            for( unsigned long i = 0; i < dobj.num_parts(); i++){
                //shift it left cannot seem to find the faces in godot
                dlib::rectangle r = dobj.get_rect();
                pts.set(i, Vector2(dobj.part(i).x()-r.left(),dobj.part(i).y()-r.top()));
            }
            emit_signal("facial_detect", Variant(Rect2(0,0,0,0)), Variant(pts));
        }
       // print_line("took X ms time processing: " + itos(os-> get_ticks_msec() -startTime));  
    }
    
}
void FacialLandmark::stopStreaming(){
    if( _state == STATUS_RUNNING ){
        _state = STATUS_STOPPED;
    }
}


