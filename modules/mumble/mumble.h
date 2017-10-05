/* mumble.h */
#ifndef MUMBLE_H
#define MUMBLE_H

#include "reference.h"
#include <mumlib.hpp>
#include "scene/resources/audio_stream_sample.h"

class Mumble :  public Reference {
    GDCLASS(Mumble,Reference);
    int count;
private:
    mumlib::MumlibConfiguration _conf;
    mumlib::Mumlib *_mum;

protected:
    static void _bind_methods();

public:
    void add(int value);
    void reset();
    int get_total() const;
    void engage(String host, int port, String user, String password);
    void setCallback( Object * o );
    void sendText(String message);
    void sendAudio(Ref<AudioStreamSample> sample);
    Mumble();
};

#endif
