/* mumble.h */
#ifndef MUMBLE_H
#define MUMBLE_H

#include "reference.h"

class Mumble : public Reference {
    GDCLASS(Mumble,Reference);

    int count;

protected:
    static void _bind_methods();

public:
    void add(int value);
    void reset();
    int get_total() const;

    Mumble();
};

#endif
