#include "treecursion_player.h"
#include "os/os.h"
#include "message_queue.h"
#include "scene/main/scene_tree.h"
#include "scene/main/node.h"
#include "scene/main/viewport.h"
#include "error_macros.h"

TreecursionPlayer *TreecursionPlayer::_singleton = nullptr;

TreecursionPlayer::TreecursionPlayer() : state(ERROR) {
	_singleton = this;
}
TreecursionPlayer::~TreecursionPlayer() {
	
}
TreecursionPlayer *TreecursionPlayer::get_singleton(){
	return _singleton;
}
void TreecursionPlayer::_bind_methods() {

}

void TreecursionPlayer::lock() {
	if (!_thread || !_mutex)
		return;
	_mutex->lock();
}
void TreecursionPlayer::unlock() {
	if (!_thread || !_mutex)
		return;
	_mutex->unlock();
}


Error TreecursionPlayer::init(){
	_thread_exited = false;
	_mutex = Mutex::create();
	_thread = Thread::create(TreecursionPlayer::thread_func, this);
	return OK;
}


void TreecursionPlayer::thread_func(void *p_udata){
	TreecursionPlayer *ac = (TreecursionPlayer *) p_udata;

	uint64_t usdelay = 20000;
	while(!ac -> _exit_thread){
		if(!ac->is_paused()) {
			ac->flush();
			OS::get_singleton()->delay_usec(usdelay);
		}
	}
}
void TreecursionPlayer::set_pause(bool paused) {
	if(state == ERROR)
		return;
	if(paused){
		state = PAUSED;
	} else {
		state = RUNNING;
	}
}
bool TreecursionPlayer::is_paused() const {
	return state != RUNNING;
}
void TreecursionPlayer::close_file() {
	reader.unref();
	state = ERROR;
}
void TreecursionPlayer::finish() {
	if (!_thread)
		return;
	_exit_thread = true;
	Thread::wait_to_finish(_thread);
	close_file();
	memdelete(_thread);
	if (_mutex)
		memdelete(_mutex);
	_thread = NULL;
}

void TreecursionPlayer::set_treecursion_reader( Ref<Treecursion> &in_reader){
	if(reader.is_valid()){
		reader.unref();
	}
	
	if(in_reader.is_valid()){
		reader = in_reader->instance_playback();
		state = PAUSED;
	} else {
		ERR_PRINTS("Treecursion HTOGG file is invalid");
	}
}
Variant TreecursionPlayer::get_init_vars() const {
	return reader->get_init_values();
}
void TreecursionPlayer::flush(){
	uint64_t current_time = OS::get_singleton()->get_ticks_usec();
	while(reader->has_next()) {
		uint64_t next_cmd_time = reader->peek_time();
		if(current_time > next_cmd_time) {
			Ref<TreecursionWriteTask> t = reader->pop();
			push_task(t);
			//print_line(t->toString());
		}else
			break;
	}
}
void TreecursionPlayer::print_all_task_time(){
	while(reader->has_next()) {
		Ref<TreecursionWriteTask> t = reader->pop();
		print_line("command time: " + itos(t->get_time()) + t->toJson());
	}
}
void TreecursionPlayer::push_task(const Ref<TreecursionWriteTask> &task){
	Node *node = NULL;
	switch(task->get_type()){
		case TreecursionWriteTask::CALL_TASK: {
			TreecursionCallTask *ct = (TreecursionCallTask *)task.ptr();

			Vector<Variant> args(ct->get_args());
			int argc = args.size();
			Vector<Variant *> argp;
			argp.resize(argc);
			for (int i = 0; i < argc; i++) {
				argp[i] = &args[i];
			}
			node = SceneTree::get_singleton()->get_root()->get_node(ct -> get_node_path());
			MessageQueue::get_singleton()->push_call(node->get_instance_id(), ct->get_name(), (const Variant **)argp.ptr(), argc, true);
			break;
		}
		case TreecursionWriteTask::SET_TASK: {
			TreecursionSetTask *st = (TreecursionSetTask *)task.ptr();
			bool valid;
			node = SceneTree::get_singleton()->get_root()->get_node(st -> get_node_path());
			MessageQueue::get_singleton()->push_set(node, st->get_name(), st ->get_value());
			break;
		}
		default:
			break;
	}

}

_TreecursionPlayer *_TreecursionPlayer::_singleton = nullptr;

void _TreecursionPlayer::set_treecursion_reader( Ref<Treecursion> in_reader) {
	TreecursionPlayer::get_singleton()->set_treecursion_reader(in_reader);
}
void _TreecursionPlayer::print_all_task_time(){
	TreecursionPlayer::get_singleton()->print_all_task_time();
}
Variant _TreecursionPlayer::get_init_values() const {
	return TreecursionPlayer::get_singleton()->get_init_vars();
}
void _TreecursionPlayer::set_pause(bool paused) {
	TreecursionPlayer::get_singleton()->set_pause(paused);
}
void _TreecursionPlayer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_pause", "enable"), &_TreecursionPlayer::set_pause);
	ClassDB::bind_method(D_METHOD("get_init_values"), &_TreecursionPlayer::get_init_values);
	ClassDB::bind_method(D_METHOD("set_treecursion", "treecursion"), &_TreecursionPlayer::set_treecursion_reader);
	ClassDB::bind_method(D_METHOD("print_all_task_time"), &_TreecursionPlayer::print_all_task_time);
}
_TreecursionPlayer::_TreecursionPlayer() {
	_singleton = this;	
}
_TreecursionPlayer::~_TreecursionPlayer() {
	_singleton = nullptr;
}