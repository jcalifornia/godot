#ifndef FACIAL_LANDMARK_LOADER_H
#define FACIAL_LANDMARK_LOADER_H

#include "io/resource_loader.h"
#include "os/file_access.h"
#include <dlib/image_processing.h>

class ResourceFormatFacialLandmark : public ResourceFormatLoader {
public:
	virtual RES load(const String &p_path, const String &p_original_path, Error *r_error = NULL);
	virtual void get_recognized_extensions(List<String> *p_extensions) const;
	virtual bool handles_type(const String &p_type) const;
	virtual String get_resource_type(const String &p_path) const;

	ResourceFormatFacialLandmark();
	virtual ~ResourceFormatFacialLandmark() {}
};

class FacialLandmarkModel : public Resource{
    GDCLASS(FacialLandmarkModel, Resource);
    
public:
    FacialLandmarkModel();
    Error set_file(const String &p_file);
private:
    dlib::shape_predictor _pose_model;
    String file;
    
};

#endif // FACIAL_LANDMARK_LOADER_H
