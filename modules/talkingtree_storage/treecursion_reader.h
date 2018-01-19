#include "resource.h"
#include "reference.h"

#ifndef TREECURSION_H
#define TREECURSION_H

#include "os/file_access.h"
#include <ogg/ogg.h>

class TreecursionReader : public Resource{
    GDCLASS(TreecursionReader, Resource);
    
public:
    TreecursionReader();
    Error set_file(const String &p_file);
private:
    String file;
    FileAccess *_file;

    ogg_sync_state state;
    
};
#endif