/* callback.h */
#ifndef CALLBACK_H
#define CALLBACK_H

#include "reference.h"

class Callback : public Reference {
    GDCLASS(Callback,Reference);

    int count;

protected:
    static void _bind_methods();

public:
    void add(int value);
    void reset();
    int get_total() const;

    Callback();
};

#endif
