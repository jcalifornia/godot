/* talking_tree_storage.cpp */

#include "talking_tree_storage.h"
#include "os/os.h"
//#include "io/treecursion_types.h"



void TalkingTreeStorage::_bind_methods() {

}


bool TalkingTreeStorage::is_empty() const {
	return this->game_queue.is_empty();
}

Ref<TreecursionWriteTask> TalkingTreeStorage::dequeue(){
	TreecursionWriteTask *ptr;
	return this->game_queue.try_pop(&ptr) ? Ref<TreecursionWriteTask>(ptr) : Ref<TreecursionWriteTask>(nullptr);
}
void TalkingTreeStorage::enqueue(TreecursionWriteTask * packet ){
	bool pushed = this->game_queue.push(packet);
	if(!pushed){
		ERR_PRINTS("Queue should not be full")
	}
}
void TalkingTreeStorage::write_task( const TreecursionWriteTask *write){
	_treecursion->write_task(*write);
}
void TalkingTreeStorage::flush(){
	while(!this->is_empty()){
		Ref<TreecursionWriteTask> packet = this->dequeue();
		if ( packet.is_valid() ) {
			this->write_task(packet.ptr());
		}else{
			//consumer caught up whatever just break
			//athough the is_empty should have stop it
			break;
		}
	}
}


//https://xiph.org/ogg/doc/rfc5334.txt

TalkingTreeStorage *TalkingTreeStorage::_singleton = NULL;
TalkingTreeStorage *TalkingTreeStorage::get_singleton() {
	return _singleton;
}
void TalkingTreeStorage::set_singleton() {
	_singleton = this;
}

void TalkingTreeStorage::new_file(){
//	_treecursion = memnew(TreecursionWriter());
	_treecursion = memnew( TreecursionWriter(itos(OS::get_singleton()->get_unix_time())) );
}

void TalkingTreeStorage::write_engine_init(const Dictionary & dict) {
	TreecursionEngineHeaderTask *task = memnew( TreecursionEngineHeaderTask(OS::get_singleton()->get_unix_time(), dict) );
	enqueue(task);
	_recording_state = INIT;
}
void TalkingTreeStorage::close_file(){
	if(_treecursion){
		memdelete(_treecursion);
		_treecursion = NULL;
	}
}
bool TalkingTreeStorage::is_running() const {
	return _recording_state == RECORDING;
}
void TalkingTreeStorage::start_recording() {
	_recording_state = RECORDING;
}
void TalkingTreeStorage::set_active(bool flag){
	if(flag){
		switch(_recording_state){
			case recording_state::INIT:
			case recording_state::STOPPED: 
				_recording_state = RECORDING;
				break;
			case recording_state::RECORDING:
				break;
			default:
				_recording_state = ERROR;
		} 
	} else {
		switch(_recording_state) {
			case recording_state::STOPPED:
			case recording_state::INIT: 
			case recording_state::FINISHED:
				break;
			case recording_state::RECORDING: 
				_recording_state = STOPPED;
				break;
			default:
				_recording_state = ERROR;
		}
	}
	ERR_PRINTS("current recording state: " + itos(_recording_state));
}
void TalkingTreeStorage::lock() {
	if (!_thread || !_mutex)
		return;
	_mutex->lock();
}
void TalkingTreeStorage::unlock() {

	if (!_thread || !_mutex)
		return;
	_mutex->unlock();
}
//https://github.com/gcp/opus-tools/blob/master/src/opusenc.c#L86


Error TalkingTreeStorage::init(){
	_thread_exited = false;
	_mutex = Mutex::create();
	_recording_state = FINISHED;
	_thread = Thread::create(TalkingTreeStorage::thread_func, this);
	return OK;
}


void TalkingTreeStorage::thread_func(void *p_udata){
	TalkingTreeStorage *ac = (TalkingTreeStorage *) p_udata;

	uint64_t usdelay = 50000;
	while(!ac -> _exit_thread){
		if(ac->is_empty()){
			ac->flush();
		}
		OS::get_singleton()->delay_usec(usdelay);
	}
}

void TalkingTreeStorage::finish() {
	if (!_thread)
		return;
	_exit_thread = true;
	Thread::wait_to_finish(_thread);
	_recording_state = FINISHED;
	get_singleton()->flush();

	close_file();
	/*
	if(_opusEncoder){
		opus_multistream_encoder_destroy(_opusEncoder);
	}*/
	
	memdelete(_thread);
	if (_mutex)
		memdelete(_mutex);
	_thread = NULL;
}


TalkingTreeStorage::TalkingTreeStorage() : _recording_state(ERROR) {
	//opusEncoder = opus_multistream_encoder_create();
	
}

TalkingTreeStorage::~TalkingTreeStorage() {
	
}

_TalkingTreeStorage *_TalkingTreeStorage::_singleton = nullptr;

void _TalkingTreeStorage::_bind_methods() {
	ClassDB::bind_method(D_METHOD("write_engine_init", "init_variables"), &_TalkingTreeStorage::write_engine_init);
	ClassDB::bind_method(D_METHOD("set_file_name", "file_name"), &_TalkingTreeStorage::set_file_name);
	ClassDB::bind_method(D_METHOD("start_recording"), &_TalkingTreeStorage::start_recording);
	ClassDB::bind_method(D_METHOD("new_file"), &_TalkingTreeStorage::new_file);
}
void _TalkingTreeStorage::start_recording() {
	TalkingTreeStorage::get_singleton()->start_recording();
}
void _TalkingTreeStorage::new_file() {
	TalkingTreeStorage::get_singleton()->new_file();
}
void _TalkingTreeStorage::write_engine_init(const Dictionary & dict){
	TalkingTreeStorage::get_singleton()->write_engine_init(dict);
}
void _TalkingTreeStorage::set_file_name(String name){
	//TalkingTreeStorage::get_singleton()->set_file_name(name);
}
_TalkingTreeStorage::_TalkingTreeStorage() {
	_singleton = this;
	TalkingTreeStorage::get_singleton()->init();
	
}
_TalkingTreeStorage::~_TalkingTreeStorage() {
	_singleton = nullptr;
}