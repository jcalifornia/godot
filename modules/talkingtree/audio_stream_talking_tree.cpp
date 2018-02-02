#include "audio_stream_talking_tree.h"
#include "print_string.h"

AudioStreamPlaybackTalkingTree::AudioStreamPlaybackTalkingTree()
			: active(false), offset(0) {

}
AudioStreamPlaybackTalkingTree::~AudioStreamPlaybackTalkingTree(){

}

void AudioStreamPlaybackTalkingTree::start(float p_from_pos) {
	active = true;
	seek(p_from_pos);
	_begin_resample();
}
void AudioStreamPlaybackTalkingTree::stop() {
	active = false;
	//base->clear();
}
bool AudioStreamPlaybackTalkingTree::is_playing() const {
	return active;
}
int AudioStreamPlaybackTalkingTree::get_loop_count() const {
	//times it looped
	return 0;
}
float AudioStreamPlaybackTalkingTree::get_playback_position() const{
	return float(offset)/base->mix_rate;
}
void AudioStreamPlaybackTalkingTree::seek(float p_time){
	//not seeking a stream	
}
float AudioStreamPlaybackTalkingTree::get_length() const {
	//if supported, otherwise return 0
	
	int len = base -> get_available_bytes();
	switch(base->format){
		case AudioStreamTalkingTree::FORMAT_8_BITS: len /= 1; break;
		case AudioStreamTalkingTree::FORMAT_16_BITS: len /= 2; break;	
	}
	if (base->stereo) {
		len /= 2;
	}
	return float(len) / float(base->mix_rate);
}

float AudioStreamPlaybackTalkingTree::get_stream_sampling_rate(){
	return base->mix_rate;
}
void AudioStreamPlaybackTalkingTree::_mix_internal(AudioFrame *p_buffer, int p_frames){
	//print_line("buffer_size: " + itos(base->get_available_bytes()));
	
	ERR_FAIL_COND(!active);
	if(!active){
		for(int i = 0; i < p_frames; i++){
			p_buffer[i] = AudioFrame(0,0);
		}
		return;
	}
	base->lock();
	int len = base -> get_available_bytes();
	switch(base->format){
		case AudioStreamTalkingTree::FORMAT_8_BITS: len /= 1; break;
		case AudioStreamTalkingTree::FORMAT_16_BITS: len /= 2; break;	
	}
	if (base->stereo) {
		len /= 2;
	}
	//print_line("checking to see if streamname: " + base->get_stream_name() + " : size " + itos(len));
	int smaller_buf = MIN(len, p_frames );
	for( int i = 0;  i < smaller_buf; i++ ){
		if(base->stereo){
			p_buffer[i] = AudioFrame( float(base->get_16())/32768.0f, float(base->get_16())/32768.0f);
		}else{
			float sample = float(base->get_16())/32768.0f;
			p_buffer[i] = AudioFrame(sample,sample);
			//print_line("0: " + rtos(sample) );
		}
	}
	base->unlock();
	offset += smaller_buf;
	int todo = p_frames - smaller_buf;
	for(int i = 0; i < todo; i++){
		p_buffer[smaller_buf + i] = AudioFrame(0, 0);
        //trying to figure out false is needed
        active = false;
	}
	
	
}
void AudioStreamTalkingTree::_bind_methods(){
	ClassDB::bind_method(D_METHOD("set_pid", "pid"), &AudioStreamTalkingTree::set_pid);
	
	ClassDB::bind_method(D_METHOD("set_format", "format"), &AudioStreamTalkingTree::set_format);	
	ClassDB::bind_method(D_METHOD("get_format"), &AudioStreamTalkingTree::get_format);
	ClassDB::bind_method(D_METHOD("clear"), &AudioStreamTalkingTree::clear);
	ClassDB::bind_method(D_METHOD("get_stream_name"), &AudioStreamTalkingTree::get_stream_name);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "format", PROPERTY_HINT_ENUM, "8-Bit,16-Bit"), "set_format", "get_format");
	ADD_SIGNAL(MethodInfo("audio_recieved"));
	BIND_ENUM_CONSTANT(FORMAT_8_BITS);
	BIND_ENUM_CONSTANT(FORMAT_16_BITS);
}
AudioStreamTalkingTree::AudioStreamTalkingTree()
	: mix_rate(44100.0), format(FORMAT_8_BITS), stereo(false), data(NULL) {
	mutex = Mutex::create();
	data = memnew(List<uint8_t>);
}
AudioStreamTalkingTree::~AudioStreamTalkingTree(){
	if(data){
		memdelete(data);
	}
}

Ref<AudioStreamPlayback> AudioStreamTalkingTree::instance_playback(){
	Ref<AudioStreamPlaybackTalkingTree> talking_tree;
	talking_tree.instance();
	talking_tree->base = Ref<AudioStreamTalkingTree>(this);
	return talking_tree;
}

Error AudioStreamTalkingTree::append_data(const uint8_t * pcm_data, int p_bytes){
	//print_line("got audio: " +  itos( data.size()));
	lock();
	for(int i = 0; i < p_bytes; i++){
		data->push_back(pcm_data[i]);
	}
	//print_line("set audio size: " + itos( data.size()));
	unlock();
	emit_signal("audio_recieved");
	return OK;
}
int AudioStreamTalkingTree::get_available_bytes() const {	
		return data->size();
}

int AudioStreamTalkingTree::get_16() {
	int16_t buf;
	uint8_t *ptr = (uint8_t *)&buf;
	
	ptr[0] = (*data)[0];
	ptr[1] = (*data)[1];
	data->pop_front();
	data->pop_front();
	return buf;
}


String AudioStreamTalkingTree::get_stream_name() const {
	return "TalkingTree: " + itos(id);
}

void AudioStreamTalkingTree::set_format(Format p_format) {
	format = p_format;
}
void AudioStreamTalkingTree::set_mix_rate(float rate){
	mix_rate = rate;
}
void AudioStreamTalkingTree::set_pid(int p_id){
	id = p_id;
}
AudioStreamTalkingTree::Format AudioStreamTalkingTree::get_format() const {
	return format;
}
void AudioStreamTalkingTree::clear(){
	data->clear();
}
void AudioStreamTalkingTree::lock(){
	mutex->lock();
}
void AudioStreamTalkingTree::unlock(){
	mutex->unlock();
}