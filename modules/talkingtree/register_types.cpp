
/* register_types.cpp */

#include "register_types.h"
#include "class_db.h"
#include "talking_tree.h"

void register_talkingtree_types() {
	 ClassDB::register_class<TalkingTree>();
}

void unregister_talkingtree_types() {
    //nothing to do here
}
