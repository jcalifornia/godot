#include <dlib/geometry/rectangle.h>
#include "math/math_2d.h"


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
    Rect2 to_gRect( dlib::rectangle & r ){
        long height = r.top() - r.bottom();
        long width = r.right() - r.left();
        return Rect2( r.left(), r.right(), width, height );
    }
}
