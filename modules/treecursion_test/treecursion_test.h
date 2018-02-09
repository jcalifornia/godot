/* talking_tree_storage.h */
#ifndef TREECURSION_TEST_STORAGE_H
#define TREECURSION_TEST_STORAGE_H



#include "object.h"
#include "variant.h"
#include "core/os/thread.h"
#include "core/os/mutex.h"

#include "reference.h"
#include "os/file_access.h"
#include "ustring.h"

#include "treecursion_write.h"

class TreecursionTestStorage : public Object {
	GDCLASS(TreecursionTestStorage, Object);

	static TreecursionTestStorage *_singleton;
	static void thread_func(void *p_udata);
public:
	static TreecursionTestStorage *get_singleton();
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
	void close_file();
	TreecursionTestStorage();
	~TreecursionTestStorage();

private:
	TreecursionTestWriter *treecursion;


};

#endif
