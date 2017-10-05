/* mumble.cpp */

#include "mumble.h"
#include "callback.h"
#include "utils.h"
#include "os/thread.h"
#include "print_string.h"
#include <mumlib/Transport.hpp>
#include <string>
#include "scene/main/timer.h"
#include <stdlib.h>



void Mumble::_bind_methods() {
    ClassDB::bind_method(D_METHOD("add", "value"), &Mumble::add);
    ClassDB::bind_method(D_METHOD("reset"), &Mumble::reset);
    ClassDB::bind_method(D_METHOD("get_total"), &Mumble::get_total);

    ClassDB::bind_method(D_METHOD("engage", "host", "port", "user", "password"), &Mumble::engage);
    ClassDB::bind_method(D_METHOD("setCallback", "callback"), &Mumble::setCallback);
    ClassDB::bind_method(D_METHOD("sendText", "text"), &Mumble::sendText);
    ClassDB::bind_method(D_METHOD("sendAudio", "sample"), &Mumble::sendAudio);

}
void Mumble::engage(String host, int port, String user, String password) {
   std::string h = utils::gstr2cpp_str(host);
   std::string u = utils::gstr2cpp_str(user);
   std::string p = utils::gstr2cpp_str(password);
   while(true){
        try{
            this->_mum->connect(h, port,  u, p);
            print_line( "Mumble: connecting to " + host );
            this->_mum->run();
        }catch (mumlib::TransportException &exp) {
            print_line( "Mumble: error " + utils::cpp_str2gstr(exp.what()));
            print_line( "Mumble: attempting to reconnect in 5s. ");
            Timer * sleep = memnew(Timer);
            sleep -> set_wait_time(5.0);
            sleep -> start();
            memdelete(sleep);
            
        }
   }
}

void Mumble::setCallback(Object * callback){
   SimpleCallback *cb = Object::cast_to<SimpleCallback>(callback);
   _mum = new mumlib::Mumlib(*(cb->get_callback()),_conf);
}
void Mumble::sendText(const String text){
    print_line("i am sending this message: " + text);
   _mum -> sendTextMessage( utils::gstr2cpp_str(text) );
}
void Mumble::sendAudio(Ref<AudioStreamSample> sample){

    const PoolByteArray data = sample->get_data();

    int32_t packet_size = 0;
    int16_t pcm[5000]; //https://github.com/slomkowski/mumlib/blob/master/src/mumlib.cpp look at lib source
       print_line("pcm_data" + itos(sample->get_format()) +" size :" + itos((int64_t)data.size()));

    if(data.size() > 0){
        switch(sample->get_format()){
            case AudioStreamSample::FORMAT_16_BITS:         

                packet_size = data.size()/2;
                for(int i = 0; i > packet_size; i++){
                    pcm[i] = data[2*i] | (data[2*i+1] << 8);
                }
                break;
            default:
                return;
        }
        _mum->sendAudioData(pcm, packet_size);
    }
    
}

Mumble::Mumble() {
    count=0;
}

void Mumble::add(int value) {

    count+=value;
}

void Mumble::reset() {

    count=0;
}


int Mumble::get_total() const {

    return count;
}
