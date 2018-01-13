/* talking_tree_storage.cpp */

#include "talking_tree_storage.h"

void TalkingTreeStorage::_bind_methods() {

}
//https://xiph.org/ogg/doc/rfc5334.txt

TalkingTreeStorage *TalkingTreeStorage::singleton = NULL;
TalkingTreeStorage *TalkingTreeStorage::get_singleton() {
	return singleton;
}
void TalkingTreeStorage::set_singleton() {
	singleton = this;
}

void TalkingTreeStorage::lock() {
	if (!thread || !mutex)
		return;
	mutex->lock();
}
void TalkingTreeStorage::unlock() {

	if (!thread || !mutex)
		return;
	mutex->unlock();
}

void TalkingTreeStorage::finish() {
	if (!thread)
		return;
	exit_thread = true;
	Thread::wait_to_finish(thread);
	memdelete(thread);
	if (mutex)
		memdelete(mutex);
	thread = NULL;
}

TalkingTreeStorage::TalkingTreeStorage() {
	//opusEncoder = opus_multistream_encoder_create();
	serialno=0;
	ogg_stream_init(&os, serialno);
}

TalkingTreeStorage::~TalkingTreeStorage() {
	if(opusEncoder){
		opus_multistream_encoder_destroy(opusEncoder);
	}
}