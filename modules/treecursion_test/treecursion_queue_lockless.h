#ifndef TREECURSION_QUEUE_LOCKLESS
#define TREECURSION_QUEUE_LOCKLESS

#include "core/object.h"

#include "ref_ptr.h"

#include <array>
#include <atomic>
/* 
 * Multiple Producer, single consumer lockless ring buffer
 * Assumes that the buffer will always have a empty spot
 * If push return false, pretend this whole progam doesnt work
 * Make the buffer larger.
 */

template<class T, uint64_t Q_SIZE>
class TreecursionQueue : public Reference{
	GDCLASS(TreecursionQueue, Object);
	
	const uint64_t Q_MASK = Q_SIZE - 1;

private:
	const uint32_t _n_producers;
	std::atomic<uint64_t> _head;
	std::atomic<uint64_t> _tail;
	std::array<std::atomic<T*>, Q_SIZE> _buffer = {};
	
public:
	bool push(T * item) {
		uint64_t head = _head.fetch_add( 1 );
		uint64_t tail = _tail.load();
		if( head >= tail + Q_SIZE ){
			/* buffer is full, just return */
			return false;
		}
		T *expected = nullptr;
		//maybe i need something different
		while(!_buffer[ head % Q_SIZE ].compare_exchange_weak( expected, item)) {

		}

		return true;
	}
	/*hmmm whatever, if it has not been committed yet
	 *
	 *  I will just get the last one on the next
	 * 	wake up
	 */
	bool pop( Ref<T> &ref ){
		uint64_t tail = _tail.load();
		T *ptr = nullptr;

		T *new_value = nullptr;
		do{

		} while ( !_buffer[tail % Q_SIZE].compare_exchange_weak( ptr, new_value ) );

		ref = Ref<T>(ptr);
		return true;

	}
	bool is_empty() {
		return this -> _head.load() == this -> _tail.load();
	}
	TreecursionQueue(uint64_t producers) : _n_producers(producers) {
		//std::atomic_init(&_head, 0);
		//std::atomic_init(&_tail, 0);
	};
	TreecursionQueue() : _n_producers(2) {
		//std::atomic_init(&_head, 0);
		//std::atomic_init(&_tail, 0);
	};
};
#endif