#include "video_object.h"

void video_object::ref() 
{
	__atomic_add_fetch(&ref_cnt, 1, __ATOMIC_SEQ_CST);
}

int video_object::unref()
{
	if (!__atomic_sub_fetch(&ref_cnt, 1, __ATOMIC_SEQ_CST)) {
		if (release_cb)
			release_cb();
		delete this;
		return 0;
	}
	return 1;
}

int video_object::refcount() 
{
	return __atomic_load_n(&ref_cnt, __ATOMIC_SEQ_CST);
}

