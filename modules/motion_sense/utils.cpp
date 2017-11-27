#include "utils.h"
#include <dlib/opencv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#define ABS(x) x > 0 ? x : -x

Rect2 utils::to_gRect( dlib::rectangle &r ){
        long height = r.top() - r.bottom();
        long width = r.right() - r.left();
        return Rect2( 0, 0, ABS(width), ABS(height) );
}

PoolVector<Vector2> utils::to_3dVec2( dlib::full_object_detection &dobj ){
    PoolVector<Vector2> pts;
    dlib::rectangle r = dobj.get_rect();
    long height = ABS(r.top() - r.bottom());
    long width = ABS(r.right() - r.left());
    pts.resize(dobj.num_parts());
    for( unsigned long i = 0; i < dobj.num_parts(); i++){
        //shift it left cannot seem to find the faces in godot
        long x = (dobj.part(i).x()-r.left());
        //long y = height - (dobj.part(i).y() - r.top());
        long y = (dobj.part(i).y()-r.top());
        pts.set(i, Vector2(x, y));
        
    }
    return pts;
}

PoolVector<Vector2> utils::to_3dVec2(dlib::full_object_detection &dobj,std::vector<cv::Point2f> &out_pts){
	PoolVector<Vector2> pts;
	dlib::rectangle r = dobj.get_rect();
    long height = ABS(r.top() - r.bottom());
    long width = ABS(r.right() - r.left());
	pts.resize(out_pts.size());
	for(unsigned long i =0; i<pts.size(); i++){
		pts.set(i,Vector2(out_pts[i].x-r.left(),out_pts[i].y-r.top()));
	}
	return pts;
		
}
