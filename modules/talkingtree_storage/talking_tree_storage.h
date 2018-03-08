/* talking_tree_storage.h */
#ifndef TALKING_TREE_STORAGE_H
#define TALKING_TREE_STORAGE_H



#include "object.h"
#include "variant.h"
#include "core/os/thread.h"
#include "core/os/mutex.h"

#include "reference.h"
#include "os/file_access.h"
#include "ustring.h"

#include "ogg/ogg.h"
#include "treecursion_writer.h"


#include <opus.h>
#include <opus_multistream.h>

class TalkingTreeStorage : public Object {
    GDCLASS(TalkingTreeStorage, Object);

    static TalkingTreeStorage *_singleton;
	static void thread_func(void *p_udata);
public:
    static TalkingTreeStorage *get_singleton();
    void set_singleton();
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

public:
    void new_file();
    void write_header();
    void close_file();
    TalkingTreeStorage();
    ~TalkingTreeStorage();

private:
    enum{
        //https://mf4.xiph.org/jenkins/view/opus/job/opus/ws/doc/html/group__opus__multistream.html
        SAMPLE_RATE = 48000,

    };
    OpusMSEncoder *opusEncoder;
    TreecursionWriter *treecursion;


};

class _TalkingTreeStorage : public Object {
	GDCLASS(_TalkingTreeStorage, Object);

	friend class TalkingTreeStorage;

	static _TalkingTreeStorage *_singleton;
protected:
	
	static void _bind_methods();
	
public:
	static _TalkingTreeStorage *get_singleton() { return _singleton; }
	void start_recording();
	void set_file_name(String name);
	_TalkingTreeStorage();
	~_TalkingTreeStorage();
};

#endif