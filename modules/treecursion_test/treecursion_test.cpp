/* talking_tree_storage.cpp */

#include "treecursion_test.h"

#include "os/os.h"

#include <atomic>


void TreecursionTestStorage::_bind_methods() {
//	ClassDB::bind_method(D_METHOD("start_recording"), &TreecursionTestStorage::startRecord);

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
void TreecursionTestStorage::flush(){
	while(!this->is_empty()){
		Ref<TreecursionWriteTask> packet = this->dequeue();
		if ( packet.is_valid() ) {
			this->write_packet(packet.ptr());
		}else{
			//consumer caught up whatever just break;
			break;
		}
	}
}
void TreecursionTestStorage::close_file(){
	flush();
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
void TreecursionTestStorage::startRecord(){
	get_singleton() -> new_file();
}
void TreecursionTestStorage::write_packet(TreecursionWriteTask *packet){
	treecursion -> write_packet(*packet);
}

void TreecursionTestStorage::enqueue(TreecursionWriteTask * packet ){
	bool pushed = this->game_queue.push(packet);
	if(!pushed){
		ERR_PRINTS("Queue should not be full")
	}
}
Ref<TreecursionWriteTask> TreecursionTestStorage::dequeue(){
	TreecursionWriteTask *ptr;
	return game_queue.try_pop(&ptr) ? Ref<TreecursionWriteTask>(ptr) : Ref<TreecursionWriteTask>(nullptr);
}
bool TreecursionTestStorage::is_empty(){
	return this->game_queue.is_empty();
}
bool TreecursionTestStorage::is_running(){
	return this->treecursion != nullptr;
}

Error TreecursionTestStorage::init(){
	_thread_exited = false;
	treecursion = nullptr;
	_mutex = Mutex::create();
	_thread = Thread::create(TreecursionTestStorage::thread_func, this);
	return OK;
}




void TreecursionTestStorage::thread_func(void *p_udata){
	TreecursionTestStorage *ac = (TreecursionTestStorage *) p_udata;
	//every 20 ms
	uint64_t usdelay = 20000;
	while(!ac -> _exit_thread){
		if(ac->treecursion != nullptr){
			ac->flush();
		}
		OS::get_singleton()->delay_usec(usdelay);
	}
}

void TreecursionTestStorage::finish() {
	if (!_thread)
		return;
	_exit_thread = true;
	Thread::wait_to_finish(_thread);
	if(treecursion)
		close_file();

	memdelete(_thread);
	if (_mutex)
		memdelete(_mutex);
	_thread = nullptr;
}


TreecursionTestStorage::TreecursionTestStorage() {
	
}

TreecursionTestStorage::~TreecursionTestStorage() {
	TreecursionTestStorage::get_singleton()->finish();
	_singleton = nullptr;
}

_TreecursionTestStorage *_TreecursionTestStorage::_singleton = nullptr;

void _TreecursionTestStorage::_bind_methods() {
	ClassDB::bind_method(D_METHOD("start_recording"), &_TreecursionTestStorage::start_recording);
	
}
void _TreecursionTestStorage::start_recording(){
	TreecursionTestStorage::get_singleton()->new_file();
}
_TreecursionTestStorage::_TreecursionTestStorage() {
	_singleton = this;
	TreecursionTestStorage::get_singleton()->init();
	
}
_TreecursionTestStorage::~_TreecursionTestStorage() {
	_singleton = nullptr;
}
