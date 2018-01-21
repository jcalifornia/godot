/* talking_tree_storage.cpp */

#include "talking_tree_storage.h"
#include "os/os.h"



void TalkingTreeStorage::_bind_methods() {
	ClassDB::bind_method(D_METHOD("close_file"), &TalkingTreeStorage::close_file);
	ClassDB::bind_method(D_METHOD("write_header"), &TalkingTreeStorage::write_header);
	ClassDB::bind_method(D_METHOD("create_file"), &TalkingTreeStorage::new_file);
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
	treecursion = memnew(TreecursionWriter);
}
void TalkingTreeStorage::write_header(){
	treecursion->write_header();
}
void TalkingTreeStorage::close_file(){
	memdelete(treecursion);
	treecursion = NULL;
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
	_thread = Thread::create(TalkingTreeStorage::thread_func, this);
	return OK;
}


void TalkingTreeStorage::thread_func(void *p_udata){
	TalkingTreeStorage *ac = (TalkingTreeStorage *) p_udata;


	//every half second.
	uint64_t usdelay = 500000;
	while(!ac -> _exit_thread){
		if(ac->treecursion != NULL){
			
		}
		OS::get_singleton()->delay_usec(usdelay);
	}
}

void TalkingTreeStorage::finish() {
	if (!_thread)
		return;
	_exit_thread = true;
	Thread::wait_to_finish(_thread);

	if(opusEncoder){
		opus_multistream_encoder_destroy(opusEncoder);
	}
	
	memdelete(_thread);
	if (_mutex)
		memdelete(_mutex);
	_thread = NULL;
}


TalkingTreeStorage::TalkingTreeStorage() {
	//opusEncoder = opus_multistream_encoder_create();
	
}

TalkingTreeStorage::~TalkingTreeStorage() {
	
}