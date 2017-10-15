/* mumble.h */
#ifndef MUMBLE_H
#define MUMBLE_H

#include "reference.h"
#include <mumlib.hpp>
#include "scene/resources/audio_stream_sample.h"
#include "variant.h"
#include "callback.h"

class Mumble :  public Reference {
    GDCLASS(Mumble,Reference);
    int count;
private:
    
    
    class _PrivateMumble: public Reference {
        GDCLASS(_PrivateMumble,Reference);
    private:
        mumlib::Mumlib _mum;
    public:
        _PrivateMumble(mumlib::Callback & c);
        void engage(String host, int port, String user, String password);
        void sendText(const String text);
        void send16bAudio(const PoolByteArray & arr);
    };
    Ref<_PrivateMumble> _pMumble;

protected:
    static void _bind_methods();

public:
    void add(int value);
    void reset();
    int get_total() const;
    void engage(String host, int port, String user, String password);
    void setCallback( Ref<SimpleCallback> c );
    void sendText(const String message);
    void sendAudio(Ref<AudioStreamSample> sample);
//    void sendAudio(PoolByteArray sample);
    Mumble();
//    ~Mumble();
};

#endif
