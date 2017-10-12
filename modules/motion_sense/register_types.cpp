#include "register_types.h"
#include "class_db.h"
#include "facial_landmark.h"
#include "facial_landmark_loader.h"
#include "data_models.h"

static ResourceFormatFacialLandmark *facial_landmark_loader = NULL;

void register_motion_sense_types(){
    ClassDB::register_class<FacialLandmark>();
    ClassDB::register_class<FacialLandmarkModel>();

    facial_landmark_loader = memnew(ResourceFormatFacialLandmark);
    ResourceLoader::add_resource_format_loader(facial_landmark_loader);

}
void unregister_motion_sense_types(){
}

