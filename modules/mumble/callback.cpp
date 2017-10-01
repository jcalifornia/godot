#include "callback.h"
#include "utils.h"
#include "variant.h"

SimpleCallback::SimpleCallback(){
}
SimpleCallback::~SimpleCallback(){
}

void SimpleCallback::audio( int target,
                       int sessionId,
                       int sequenceNumber,
                       int16_t *pcm_data,
                        uint32_t pcm_data_size){
   int i = 0;
}


void SimpleCallback::textMessage(
            uint32_t actor,
            std::vector<uint32_t> session,
            std::vector<uint32_t> channel_id,
            std::vector<uint32_t> tree_id,
	    std::string message) {

   Variant *s = utils::cpp_vec2garr(session);
   Variant *c = utils::cpp_vec2garr(channel_id);
   Variant *t = utils::cpp_vec2garr(tree_id);
   Variant *a = memnew( Variant(actor) );
   Variant *m = memnew( Variant(String(message.c_str())));
   Variant::CallError err;
   const Variant *args[5] = {a, s, c, t, m};
   Variant result =  this->text_handler->call_func( args, 5, err );
   memdelete( a);
   memdelete( s);
   memdelete( c);
   memdelete( t);
   memdelete( m);

}
void SimpleCallback::_bind_methods(){
   ClassDB::bind_method(D_METHOD("setAudioHandler", "handler"), &SimpleCallback::setAudioHandler);
   ClassDB::bind_method(D_METHOD("setTextHandler", "handler"), &SimpleCallback::setTextHandler);
}
void SimpleCallback::setAudioHandler( Object * handler){
   this->audio_handler = (FuncRef *)handler;
}
void SimpleCallback::setTextHandler( Object * handler){
   this->text_handler = (FuncRef *)handler;
}
