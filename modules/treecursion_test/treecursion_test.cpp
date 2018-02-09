/* talking_tree_storage.cpp */

#include "treecursion_test.h"

#include "os/os.h"



void TreecursionTestStorage::_bind_methods() {
	ClassDB::bind_method(D_METHOD("close_file"), &TreecursionTestStorage::close_file);
}

TreecursionTestStorage *TreecursionTestStorage::_singleton = NULL;
TreecursionTestStorage *TreecursionTestStorage::get_singleton() {
	return _singleton;
}
void TreecursionTestStorage::set_singleton() {
	_singleton = this;
}
void TreecursionTestStorage::new_file(){
	treecursion = memnew(TreecursionTestWriter);
}
void TreecursionTestStorage::close_file(){
	memdelete(treecursion);
	treecursion = NULL;
}
void TreecursionTestStorage::lock() {
	if (!_thread || !_mutex)
		return;
	_mutex->lock();
}
void TreecursionTestStorage::unlock() {

	if (!_thread || !_mutex)
		return;
	_mutex->unlock();
}
void TreecursionTestStorage::write_packet(TreecusionWriteTask & packet){
	treecursion -> write_packet(packet);
}

Error TreecursionTestStorage::init(){
	_thread_exited = false;
	_mutex = Mutex::create();
	_thread = Thread::create(TreecursionTestStorage::thread_func, this);
	return OK;
}


void TreecursionTestStorage::thread_func(void *p_udata){
	TreecursionTestStorage *ac = (TreecursionTestStorage *) p_udata;


	//every half second.
	uint64_t usdelay = 500000;
	while(!ac -> _exit_thread){
		if(ac->treecursion != NULL){
			
		}
		OS::get_singleton()->delay_usec(usdelay);
	}
}

void TreecursionTestStorage::finish() {
	if (!_thread)
		return;
	_exit_thread = true;
	Thread::wait_to_finish(_thread);

	
	memdelete(_thread);
	if (_mutex)
		memdelete(_mutex);
	_thread = NULL;
}


TreecursionTestStorage::TreecursionTestStorage() {
	
}

TreecursionTestStorage::~TreecursionTestStorage() {
	
}
