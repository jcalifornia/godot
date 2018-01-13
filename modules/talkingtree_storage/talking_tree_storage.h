/* talking_tree_storage.h */
#ifndef TALKING_TREE_STORAGE_H
#define TALKING_TREE_STORAGE_H



#include "object.h"
#include "variant.h"
#include "core/os/thread.h"
#include "core/os/mutex.h"

#include "reference.h"
#include "os/file_access.h"


#include "ogg/ogg.h"
#include <kate/oggkate.h>


#include <opus.h>
#include <opus_multistream.h>

class TalkingTreeStorage : public Object {
    GDCLASS(TalkingTreeStorage, Object);

    static TalkingTreeStorage *singleton;
	static void thread_func(void *p_udata);
public:
    static TalkingTreeStorage *get_singleton();
    void set_singleton();
    void lock();
	void unlock();
	void finish();
	Error init();

private:
	bool thread_exited;
	mutable bool exit_thread;
    Thread *thread;
	Mutex *mutex;
    
protected:
    static void _bind_methods();

public:
    TalkingTreeStorage();
    ~TalkingTreeStorage();

private:
    enum{
        //https://mf4.xiph.org/jenkins/view/opus/job/opus/ws/doc/html/group__opus__multistream.html
        SAMPLE_RATE = 48000,

    };
    OpusMSEncoder *opusEncoder;
    FileAccess *_fa;
    ogg_stream_state os;
    ogg_packet op;
    ogg_page og; /* one Ogg bitstream page. */
    int serialno;


};

#endif