#include "resource.h"
#include "reference.h"

#ifndef TREECURSIONTESTREADER_H
#define TREECURSIONTESTREADER_H

#include "os/file_access.h"
#include "io/treecursion_types.h"



class TreecursionTestReader : public Resource{
    GDCLASS(TreecursionTestReader, Resource);
    
public:   
    Error set_file(const String &p_file);
	Ref<TreecursionWriteTask> next();
    TreecursionTestReader();
	bool eof_reached(){
		return _file->eof_reached();
	}

protected:
    static void _bind_methods();

private:
    String file;
    FileAccess *_file;

    
};
#endif