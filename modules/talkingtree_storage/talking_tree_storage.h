/* talking_tree_storage.h */
#ifndef TALKING_TREE_STORAGE_H
#define TALKING_TREE_STORAGE_H

#include "reference.h"

#include "ogg/ogg.h"

class TalkingTreeStorage : public Reference {
    GDCLASS(TalkingTreeStorage, Reference);


protected:
    static void _bind_methods();

public:
    TalkingTreeStorage();
};

#endif