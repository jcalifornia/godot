#include "treecursion.h"

#include "os/file_access.h"



Error Treecursion::set_file(const String &p_file) {
    file = p_file;
	Error err;
	FileAccess *f = FileAccess::open(file, FileAccess::READ, &err);
	if (err) {
		ERR_FAIL_COND_V(err, err);
	}
	int _err;

    memdelete(f);
}

Treecursion::Treecursion() {
    
}
