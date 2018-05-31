#ifndef __DEBUG_H_
#define __DEBUG_H_

#ifndef DEBUG
#define DEBUG
#endif

#ifdef DEBUG
#include <assert.h>
#else
#undef assert
#define assert(A)
#endif

#ifdef DEBUG
#define PRINTF_FUNC \
		printf("%s, thread: 0x%x\n", __func__, (int)pthread_self())

#define PRINTF_FUNC_OUT \
  		printf("---- OUT OF %s, thread: 0x%x ----\n", __func__, (int)pthread_self())

#define PRINTF_FUNC_LINE \
		printf("~~ %s: %d\n", __func__, __LINE__)

#define PRINTF(...) \
		printf(__VA_ARGS__)


#define func_trace(fmt, args...) \
		printf("[%s]:>>>>>" fmt,__FUNCTION__, ##args)

#define func_func(fmt, args...) printf("[%s]:<<<<<\n", __FUNCTION__)

#else
#define PRINTF_FUNC
#define PRINTF_FUNC_OUT
#define PRINTF_FUNC_LINE
#define PRINTF(...)
#define func_trace(fmt, ...)
#define func_func(fmt, ...)
#endif

#define PRINTF_NO_MEMORY                              \
  		printf("no memory! %s : %d\n", __func__, __LINE__); \
  		assert(0)
  		
#define likely(x) __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)


#endif

