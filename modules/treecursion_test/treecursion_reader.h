#ifndef TREECURSIONTESTREADER_H
#define TREECURSIONTESTREADER_H

#include "resource.h"
#include "reference.h"

#include "vector.h"
#include "os/file_access.h"
#include "io/treecursion_types.h"


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