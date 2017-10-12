#include "facial_landmark_loader.h"

ResourceFormatFacialLandmark::ResourceFormatFacialLandmark() {
}
RES ResourceFormatFacialLandmark::load(const String &p_path, const String &p_original_path, Error *r_error) {
	if (r_error)
		*r_error = OK;

	FacialLandmarkModel *facial_landmark = memnew(FacialLandmarkModel);
	facial_landmark->set_file(p_path);
	return Ref<FacialLandmarkModel>(facial_landmark);
}

void ResourceFormatFacialLandmark::get_recognized_extensions(List<String> *p_extensions) const {

	p_extensions->push_back("dat");
}
String ResourceFormatFacialLandmark::get_resource_type(const String &p_path) const {

	if (p_path.get_extension().to_lower() == "dat")
		return "FacialLandmarkModel";
	return "";
}

bool ResourceFormatFacialLandmark::handles_type(const String &p_type) const {
	return (p_type == "FacialLandmarkModel");
}

Error FacialLandmarkModel::set_file(const String &p_file) {
    file = p_file;
	Error err;
	Vector<uint8_t> byteData = FileAccess::get_file_as_array(file);
	if (err) {
		ERR_FAIL_COND_V(err, err);
	}

    int _err;
    //http://dlib.net/dlib/serialize.h.html
    std::stringstream ss;
    ss <<  byteData.ptr();
    dlib::deserialize(_pose_model, ss);
}

FacialLandmarkModel::FacialLandmarkModel() {
    
}
