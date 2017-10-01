/* callback.cpp */

#include "callback.h"

void Callback::add(int value) {

    count+=value;
}

void Callback::reset() {

    count=0;
}

int Callback::get_total() const {

    return count;
}

void Callback::_bind_methods() {
    ClassDB::bind_method(D_METHOD("add", "value"), &Callback::add);
    ClassDB::bind_method(D_METHOD("reset"), &Callback::reset);
    ClassDB::bind_method(D_METHOD("get_total"), &Callback::get_total);

}

Callback::Callback() {
    count=0;
}

