#include "utils.h"

#define ABS(x) x > 0 ? x : -x

Rect2 utils::to_gRect( dlib::rectangle & r ){
        long height = r.top() - r.bottom();
        long width = r.right() - r.left();
        return Rect2( 0, 0, ABS(width), ABS(height) );
}
