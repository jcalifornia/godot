#include "resource.h"
#include "reference.h"

#ifndef TREECURSION_H
#define TREECURSION_H


class Treecursion : public Resource{
    GDCLASS(Treecursion, Resource);
    
public:
    Treecursion();
    Error set_file(const String &p_file);
private:
    String file;
    
};
#endif