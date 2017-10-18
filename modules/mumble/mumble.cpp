/* mumble.cpp */

#include "mumble.h"
#include "callback.h"
#include "utils.h"
#include "os/thread.h"
#include "print_string.h"
#include <mumlib/Transport.hpp>
#include <string>
#include "scene/main/timer.h"

Mumble::_PrivateMumble::_PrivateMumble( mumlib::Callback & c) : _mum(c) {
}
void Mumble::_PrivateMumble::engage(String host, int port, String user, String password){
	std::string h = utils::gstr2cpp_str(host);
	std::string u = utils::gstr2cpp_str(user);
	std::string p = utils::gstr2cpp_str(password);
	while(true){
		try{
			this->_mum.connect(h, port,  u, p);
			print_line( "Mumble: connecting to " + host );
			this->_mum.run();
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

void Mumble::_PrivateMumble::sendText(const String text){
	_mum.sendTextMessage( utils::gstr2cpp_str(text) );
}

void Mumble::_PrivateMumble::send16bAudio(const PoolByteArray & sample){
	uint32_t pcm_len = sample.size()/2;
	int16_t pcm[50000];
	for(int i = 0; i < pcm_len; i++){
		uint16_t low = (uint16_t) sample[2*i];
		uint16_t hi = (uint16_t) sample[2*i+1];
		pcm[i] = ( low | ( hi << 8));
	}
	_mum.sendAudioData(pcm, pcm_len);
}

void Mumble::_bind_methods() {
	ClassDB::bind_method(D_METHOD("engage", "host", "port", "user", "password"), &Mumble::engage);
	ClassDB::bind_method(D_METHOD("setCallback", "callback"), &Mumble::setCallback);
	ClassDB::bind_method(D_METHOD("sendText", "text"), &Mumble::sendText);
	ClassDB::bind_method(D_METHOD("sendAudio", "sample"), &Mumble::sendAudio);

}
void Mumble::engage(String host, int port, String user, String password) {
	_pMumble -> engage( host,  port,  user,  password);
}

void Mumble::setCallback( Ref<SimpleCallback> cb){

	_pMumble = Ref<_PrivateMumble>(memnew(_PrivateMumble(*(cb->get_callback()))));
}
void Mumble::sendText(const String text){
	_pMumble -> sendText( text );
}

void Mumble::sendAudio(Ref<AudioStreamSample> sample){

	const PoolByteArray data = sample->get_data();


	if(data.size() > 0){
		switch(sample->get_format()){
			case AudioStreamSample::FORMAT_16_BITS:         
				_pMumble->send16bAudio(data);
			default:
				return;
		}
	}
    
}

Mumble::Mumble() {
}
