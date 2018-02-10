/* talking_tree_storage.cpp */

#include "treecursion_test.h"

#include "os/os.h"

#include <atomic>


void TreecursionTestStorage::_bind_methods() {
	ClassDB::bind_method(D_METHOD("close_file"), &TreecursionTestStorage::close_file);
}

TreecursionTestStorage *TreecursionTestStorage::_singleton = nullptr;
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
	treecursion = nullptr;
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
void TreecursionTestStorage::write_packet(TreecursionWriteTask *packet){
	treecursion -> write_packet(*packet);
}

void TreecursionTestStorage::enqueue(TreecursionWriteTask * packet ){
	bool pushed = game_queue.push(packet);
	if(!pushed){
		ERR_PRINTS("Queue should not be full")
	}
}
Ref<TreecursionWriteTask> TreecursionTestStorage::dequeue(){
	Ref<TreecursionWriteTask> pack;
	game_queue.pop(pack);
	return pack;
}
bool TreecursionTestStorage::is_empty(){
	return game_queue.is_empty();
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
		if(ac->treecursion != nullptr){
			while(!ac->is_empty()){
				Ref<TreecursionWriteTask> packet = ac->dequeue();
				ac->write_packet(packet.ptr());
			}
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
	_thread = nullptr;
}


TreecursionTestStorage::TreecursionTestStorage() {
	
}

TreecursionTestStorage::~TreecursionTestStorage() {
	
}
