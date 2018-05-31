#ifndef __VIDEO_OBJECT_H_
#define __VIDEO_OBJECT_H_

#include <condition_variable>
#include <functional>
#include <list>
#include <mutex>

#include <assert.h>

#define UPALIGNTO(value, align) ((value + align - 1) & (~(align - 1)))
#define UPALIGNTO16(value) UPALIGNTO(value, 16)

class video_object {
private:
	volatile int ref_cnt;

public:
	std::function<void()> release_cb;
	video_object() : ref_cnt(1) {}
	virtual ~video_object() {
		// printf("%s: %p\n", __func__, this);
		assert(ref_cnt == 0);
	}
	void ref();
	int unref();  // return 0 means object is deleted
	int refcount();
	void resetRefcount() {
		//__atomic_store_n(&ref_cnt, 0, __ATOMIC_SEQ_CST);
		ref_cnt = 0;
	}
};


#endif
