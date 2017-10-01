/* register_types.cpp */

#include "register_types.h"
#include "class_db.h"
#include "mumble.h"
#include "callback.h"

void register_mumble_types() {

        ClassDB::register_class<Mumble>();
        ClassDB::register_class<Callback>();
}

void unregister_mumble_types() {
   //nothing to do here
}

