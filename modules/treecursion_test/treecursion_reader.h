#include "resource.h"
#include "reference.h"

#ifndef TREECURSIONTESTREADER_H
#define TREECURSIONTESTREADER_H

#include "os/file_access.h"
#include "io/treecursion_types.h"



class TreecursionTestReader : public Resource{
    GDCLASS(TreecursionTestReader, Resource);
    
public:
    bool eof_reached(){
		return _file->eof_reached();
	}
    Error set_file(const String &p_file);
	Ref<TreecursionWriteTask> next();
    TreecursionTestReader();
    ~TreecursionTestReader();
	

protected:
    static void _bind_methods();

private:
    String file;
    FileAccess *_file;

    
};

class TreecursionTestInit : public Resource{
    GDCLASS(TreecursionTestInit, Resource);

protected:
    static void _bind_methods();

private:
    Dictionary dict;
public:
    Variant get_dict();
    TreecursionTestInit(const Dictionary &d);
    TreecursionTestInit();
    ~TreecursionTestInit();

    
};
#endif