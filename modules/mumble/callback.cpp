#include "callback.h"
#include "utils.h"
#include "variant.h"
#include "print_string.h"

SimpleCallback::SimpleCallback(){
}
SimpleCallback::~SimpleCallback(){
}

void SimpleCallback::audio( int target,
                       int sessionId,
                       int sequenceNumber,
                       int16_t *pcm_data,
                       uint32_t pcm_data_size){
   if(_audio_handler != NULL){
    Variant tar = memnew( Variant(target) );
    Variant sid = memnew( Variant(sessionId) );
    Variant snum = memnew( Variant(sequenceNumber) );
    Variant *pcm = utils::short2byte(pcm_data, pcm_data_size);
    Variant::CallError err;
    const Variant *args[4] = {&tar, &sid, &snum, pcm};
    Variant result = _audio_handler->call_func( args, 4, err);
    memdelete( pcm);
   }
}


void SimpleCallback::textMessage(
            uint32_t actor,
            std::vector<uint32_t> session,
            std::vector<uint32_t> channel_id,
            std::vector<uint32_t> tree_id,
	    std::string message) {
   if(this->_text_handler != NULL){
       print_line("internal message: " + String(message.c_str()) );
        print_line( session.size() + ":" +channel_id.size() );
        Variant s = utils::cpp_uint32vec2Variant(session);
        Variant c = utils::cpp_uint32vec2Variant(channel_id);
        Variant t = utils::cpp_uint32vec2Variant(tree_id);
        Variant a = Variant(actor);
        Variant m = Variant(String(message.c_str()));
        Variant::CallError err;
        const Variant *args[5] = {&a, &s, &c, &t, &m};
        Variant result =  this->_text_handler->call_func( args, 5, err );
   }

}
void SimpleCallback::version(
                uint16_t major,
                uint8_t minor,
                uint8_t patch,
                std::string release,
                std::string os,
                std::string os_version){
      print_line("processing os: " + String(os.c_str()) );
}
void SimpleCallback::_bind_methods(){
   ClassDB::bind_method(D_METHOD("setAudioHandler", "handler"), &SimpleCallback::setAudioHandler);
   ClassDB::bind_method(D_METHOD("setTextHandler", "handler"), &SimpleCallback::setTextHandler);
}
void SimpleCallback::setAudioHandler( Object * handler){
   this->_audio_handler = Object::cast_to<FuncRef>(handler);
}
void SimpleCallback::setTextHandler( Object * handler){
   this->_text_handler = Object::cast_to<FuncRef>(handler);
}
