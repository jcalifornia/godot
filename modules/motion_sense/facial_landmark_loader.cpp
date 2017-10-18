#include "facial_landmark_loader.h"
#include "data_models.h"

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

	p_extensions->push_back("f68");
}
String ResourceFormatFacialLandmark::get_resource_type(const String &p_path) const {

	if (p_path.get_extension().to_lower() == "f68")
		return "FacialLandmarkModel";
	return "";
}

bool ResourceFormatFacialLandmark::handles_type(const String &p_type) const {
	return (p_type == "FacialLandmarkModel");
}
