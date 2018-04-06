#ifndef TREECURSION_PLAYER_H
#define TREECURSION_PLAYER_H

#include "object.h"
#include "variant.h"
#include "core/os/thread.h"
#include "core/os/mutex.h"
#include "treecursion_reader.h"
#include "modules/talkingtree/audio_stream_talking_tree.h"

class TreecursionPlayer : public Object {
	GDCLASS(TreecursionPlayer, Object);

	static TreecursionPlayer *_singleton;
	static void thread_func(void *p_udata);
public:
	static TreecursionPlayer *get_singleton();
	void lock();
	void unlock();
	void finish();
	Error init();

private:
	bool _thread_exited;
	mutable bool _exit_thread;
	Thread *_thread;
	Mutex *_mutex;

protected:
	static void _bind_methods();

private:
	enum PLAYING_STATE {
		ERROR,
		PAUSED,
		RUNNING,
	};
	PLAYING_STATE state;
	Ref<TreecursionReader> reader;
	HashMap<int, Ref<AudioStreamTalkingTree>> reply_audio_stream_peers;
	void push_task(const Ref<TreecursionWriteTask> &task);
	void flush();

public:
	void close_file();
	bool is_paused() const;
	void set_pause(bool paused);
	void set_pid(int pid);
	Ref<AudioStreamTalkingTree> get_audio_stream_peer(int pid);
	Variant get_init_vars() const;
	void set_treecursion_reader(Ref<Treecursion> &file);
	void print_all_task_time();
	TreecursionPlayer();
	~TreecursionPlayer();
	
};

class _TreecursionPlayer : public Object {
	GDCLASS(_TreecursionPlayer, Object);

	friend class TreecursionPlayer;
	static _TreecursionPlayer *_singleton;

protected:
	static void _bind_methods();

public:
	static _TreecursionPlayer *get_singleton() { return _singleton; }
	Variant get_init_values() const;
	void print_all_task_time();
	void set_pause(bool paused);
	void set_treecursion_reader(Ref<Treecursion> in_reader);
	Ref<AudioStreamTalkingTree> get_audio_stream_peer(int pid);
	void set_pid(int pid);
	_TreecursionPlayer();
	~_TreecursionPlayer();
};

#endif