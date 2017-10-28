/* facial_landmark.cpp */

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/opencv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

#include <opencv2/video/tracking.hpp>
#include <vector>
#include "facial_landmark.h"
#include "utils.h"
#include "variant.h"
#include "math/math_2d.h"
#include "print_string.h"
#include "dvector.h"
#include "os/os.h"


/***************************************************************
 * dlib/OpenCV stuff here
 **************************************************************/


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
	const int points_num = 68;
	const int stateNum = points_num *4;
	const int measureNum = points_num *2;
	bool started = 0;

	cv::KalmanFilter KF(stateNum, measureNum, 0);
	cv::Mat state(stateNum, 1, CV_32FC1);
	cv::Mat processNoise(stateNum, 1, CV_32F);
	cv::Mat measurement = cv::Mat::zeros(measureNum, 1, CV_32F);

	randn(state, cv::Scalar::all(0), cv::Scalar::all(0.0));

	// Generate the Measurement Matrix
	KF.transitionMatrix = cv::Mat::zeros(stateNum, stateNum, CV_32F);
	for (int i = 0; i < stateNum; i++) {
		for (int j = 0; j < stateNum; j++) {
			if (i == j || (j - measureNum) == i) {
				KF.transitionMatrix.at<float>(i, j) = 1.0;
			} else {
				KF.transitionMatrix.at<float>(i, j) = 0.0;
			}   
		}
	}
	
	setIdentity(KF.measurementMatrix);

	//!< process noise covariance matrix (Q)  
	setIdentity(KF.processNoiseCov, cv::Scalar::all(1e-5));
	  
	//!< measurement noise covariance matrix (R)  
	setIdentity(KF.measurementNoiseCov, cv::Scalar::all(1e-1));

	//!< priori error estimate covariance matrix
	setIdentity(KF.errorCovPost, cv::Scalar::all(1));

	randn(KF.statePost, cv::Scalar::all(0), cv::Scalar::all(0.0));

	// Initialize Optical Flow
	cv::Mat prevgray, gray;
	std::vector<cv::Point2f> prevTrackPts(points_num);
	std::vector<cv::Point2f> nextTrackPts(points_num);
	for (int i = 0; i < points_num; i++) {
		prevTrackPts[i] = cv::Point2f(0, 0);
	}
	
	
	std::vector<cv::Point2f> predict_points(points_num);
	std::vector<cv::Point2f> kalman_points(points_num);

	
	if(!_vc.isOpened()){
		ERR_PRINT("FacialLandmark: unable to connect to camera\n");
		_state = STATUS_ERROR;
	}
	_state = STATUS_RUNNING;
	dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
	int counter = 0;
	while( _state == STATUS_RUNNING ){
		cv::Mat temp, temp_small;
		
		if (!_vc.read(temp)){
			ERR_PRINT("FacialLandmark: unable to read to camera\n");
			break;
		}
		

		
		
		if(counter==SKIP_FRAMES){
			cv::resize(temp,temp_small, cv::Size(), 1.0/FACE_DOWNSAMPLE_RATIO, 1.0/FACE_DOWNSAMPLE_RATIO);
			dlib::cv_image<dlib::bgr_pixel> cimg(temp_small);
			cv::Mat cloned = temp_small.clone();

			std::vector<dlib::rectangle> faces = detector(cimg);
			print_line("delected x faces: " + itos(faces.size()));
			std::vector<dlib::full_object_detection> shapes(faces.size());
			std::vector<dlib::rectangle> rects(faces.size());
			if(faces.size()>0) { // @TODO: Multiple faces
				for (unsigned long i = 0; i < faces.size(); i++) {
					// this is the landmark detector
					shapes[i] = (_model->get_data())(cimg, faces[i]);
					rects[i] = shapes[i].get_rect();
				}
				PoolVector<Vector2> pts = utils::to_3dVec2(shapes[0]); // just use the first one
				print_line("sending landmarks");
				emit_signal("facial_detect", Variant(utils::to_gRect(rects[0])), Variant(pts));
			}
			
			
			// Filter stuff goes below!
			///*
			dlib::full_object_detection& shape = shapes[0];
			if (!started) {
				cv::cvtColor(cloned, prevgray, CV_BGR2GRAY);
				for (int i = 0; i < shape.num_parts(); i++) {
					prevTrackPts[i].x = shape.part(i).x();
					prevTrackPts[i].y = shape.part(i).y();
				}
				started = 1; 
			}
			
			// update filter observations
			if (shapes.size() == 1) {
				for (int i = 0; i < shape.num_parts(); i++) {
					kalman_points[i].x = shape.part(i).x();
					kalman_points[i].y = shape.part(i).y();
				}
			}

			// update prediction
			cv::Mat prediction = KF.predict();
			for (int i = 0; i < points_num; i++) {
				predict_points[i].x = prediction.at<float>(i * 2);
				predict_points[i].y = prediction.at<float>(i * 2 + 1);
			}
			//* */
			
			counter=0;
		}
		else{
			counter++;
		}
	}
	
}
void FacialLandmark::stopStreaming(){
	if( _state == STATUS_RUNNING ){
		_state = STATUS_STOPPED;
	}
}


