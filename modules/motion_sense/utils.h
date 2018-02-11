#include <dlib/geometry/rectangle.h>
#include <dlib/image_processing/full_object_detection.h>
#include <dlib/opencv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include "math/math_2d.h"
#include "dvector.h"

/*images seem to be lefthanded such as
 * 0123
 * 1
 * 2
 * 3
 * 
 * godot3d should be right handed
 * 
 * 
 * need to translate between the two
 */ 
namespace utils{
    //http://introcomputing.org/image-introduction.html
    //2d and 3d coordate system are seperate in godot
    //https://github.com/GodotNativeTools/godot-cpp/blob/master/include/core/Transform2D.hpp#L22
    //http://docs.godotengine.org/en/latest/classes/class_rect2.html
    //http://dlib.net/dlib/geometry/rectangle.h.html
    Rect2 to_gRect( dlib::rectangle &r );
    PoolVector<Vector2> to_3dVec2( dlib::full_object_detection &obj);
    PoolVector<Vector2> to_3dVec2(dlib::full_object_detection &dobj, std::vector<cv::Point2f> &out_pts);
    
}
