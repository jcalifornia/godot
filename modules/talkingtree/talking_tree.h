/* talking_tree.h */
#ifndef TALKING_TREE_H
#define TALKING_TREE_H

#include "reference.h"
#include "TalkingTree.pb.h"

class TalkingTree : public Reference {
    GDCLASS(TalkingTree, Reference);

    int count;

protected:
    static void _bind_methods();

public:
    TalkingTree();
};

#endif
