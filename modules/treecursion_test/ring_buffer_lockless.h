#include "core/object.h"
#include "core/safe_refcount.h"


#include <atomic>
/* 
 * Multiple Producer, single consumer lockless ring buffer
 * 
 */

template<class T, uint64_t Q_SIZE>
class TreecursionQueue : public Object{
	GDCLASS(TreecursionQueue, Object);
	
	static const uint64_t Q_MASK = Q_SIZE - 1;

private:
	const size_t n_producers:
	std::atomic<size_t> head;
	std::atomic<size_t> tail;
	T buffer[Q_SIZE];
	
	
public:
	bool push(const T & item){
		return false;
	};
	T *pop(){
		return nullptr;
	};
	bool is_empty() {
		return head.load() == tail.load();
	}
	TreecursionQueue(size_t producers) : n_producers(producers), head(0), tail(0){

	}

};