/* talking_tree_storage.cpp */

#include "talking_tree_storage.h"
#include "os/os.h"

void TalkingTreeStorage::_bind_methods() {

}
//https://xiph.org/ogg/doc/rfc5334.txt

TalkingTreeStorage *TalkingTreeStorage::_singleton = NULL;
TalkingTreeStorage *TalkingTreeStorage::get_singleton() {
	return _singleton;
}
void TalkingTreeStorage::set_singleton() {
	_singleton = this;
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
int TalkingTreeStorage::oe_write_page(ogg_page *page, FileAccess *fa){
	fa->store_buffer(page->header, page->header_len);
	fa->store_buffer(page->body, page->body_len);
	return page->header_len + page->body_len;
}

Error TalkingTreeStorage::init(){
	serialno=0;
	ogg_stream_init(&os, serialno);
	//create a file using unix time as file name by default
	int current_time = OS::get_singleton()->get_unix_time();


	_outfile_name = itos(current_time) + ".htogg";
	_fout = FileAccess::open(_outfile_name, FileAccess::WRITE);



	_thread_exited = false;
	_mutex = Mutex::create();
	_thread = Thread::create(TalkingTreeStorage::thread_func, this);
	return OK;
}
void TalkingTreeStorage::thread_func(void *p_udata){
	TalkingTreeStorage *ac = (TalkingTreeStorage *) p_udata;
	uint64_t usdelay = 16000;
	while(!ac -> _exit_thread){
		OS::get_singleton()->delay_usec(usdelay);
	}
}

void TalkingTreeStorage::finish() {
	if (!_thread)
		return;
	_exit_thread = true;
	Thread::wait_to_finish(_thread);
	//
	ogg_stream_flush( &os, &og);

	_fout->close();
	//
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