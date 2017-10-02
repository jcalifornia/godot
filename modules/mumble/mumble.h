/* mumble.h */
#ifndef MUMBLE_H
#define MUMBLE_H

#include "reference.h"
#include <mumlib.hpp>

class Mumble :  public Reference {
    GDCLASS(Mumble,Reference);
    int count;
private:
    mumlib::MumlibConfiguration conf;
    mumlib::Mumlib *mum;

protected:
    static void _bind_methods();

public:
    void add(int value);
    void reset();
    int get_total() const;
    void engage(String host, int port, String user, String password);
    void setCallback( Object * o );

    Mumble();
};

#endif
