#include "data_models.h"
#include <istream>
#include <streambuf>
#include <cstdio>
#include "vector.h"
#include "os/file_access.h"

class GodotFileInStreamBuf : public std::streambuf{
public:
    GodotFileInStreamBuf(FileAccess * fa) {
        _file = fa;
    }
    int underflow(){
        if (_file->eof_reached()){
            return EOF;
        }else{
            size_t pos = _file->get_position();
            uint8_t ret = _file->get_8();
            _file->seek(pos);
            return ret;
        }
    }
    int uflow(){
        return _file->eof_reached() ?  EOF : _file -> get_8();
    }
private:
    FileAccess * _file;
};

Error FacialLandmarkModel::set_file(const String &p_file) {
    file = p_file;
	Error err;
	FileAccess *f = FileAccess::open(file, FileAccess::READ, &err);
	if (err) {
		ERR_FAIL_COND_V(err, err);
	}
	int _err;
    GodotFileInStreamBuf sBuf(f);
    std::istream is(&sBuf);
   //http://dlib.net/dlib/serialize.h.html
    
    dlib::deserialize(_pose_model, is);
    memdelete(f);
}

FacialLandmarkModel::FacialLandmarkModel() {
    
}
