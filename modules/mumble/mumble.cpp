/* mumble.cpp */

#include "mumble.h"

void Mumble::add(int value) {

    count+=value;
}

void Mumble::reset() {

    count=0;
}

int Mumble::get_total() const {

    return count;
}

void Mumble::_bind_methods() {
    ClassDB::bind_method(D_METHOD("add", "value"), &Mumble::add);
    ClassDB::bind_method(D_METHOD("reset"), &Mumble::reset);
    ClassDB::bind_method(D_METHOD("get_total"), &Mumble::get_total);

}

Mumble::Mumble() {
    count=0;
}

