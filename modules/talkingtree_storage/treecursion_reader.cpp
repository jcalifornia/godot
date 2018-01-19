#include "treecursion_reader.h"


//https://bluishcoder.co.nz/2009/06/24/reading-ogg-files-using-libogg.html


Error TreecursionReader::set_file(const String &p_file) {
    file = p_file;
	Error err;
	_file = FileAccess::open(file, FileAccess::READ, &err);
	if (err) {
		ERR_FAIL_COND_V(err, err);
	}
	int ret = ogg_sync_init(&state);
	
	return OK;
}

TreecursionReader::TreecursionReader() {
    
}
