
/**
 * @file psort.c
 *
 * @brief parallel sort library frontend implementation
 *
 * @author Hajime Suzuki
 * @date 2016/3/20
 * @license MIT
 */

#include <stdint.h>
#include <string.h>
// #include "util.h"
#include "arch/arch.h"
#include "psort.h"
#include "ptask/ptask.h"		/* pthread parallel task execution library */
#include "log.h"

/* import unittest */
#define UNITTEST_UNIQUE_ID			200

#ifdef TEST
/* use auto-generated main function to run tests */
#define UNITTEST 					1
#define UNITTEST_ALIAS_MAIN			1
#endif

#include  "unittest.h"


/* constants */
#define WCR_OCC_SIZE				( 1<<8 )	/** 8bit */

/**
 * @macro _likely, _unlikely
 * @brief branch prediction hint for gcc-compatible compilers
 */
#define _likely(x)		__builtin_expect(!!(x), 1)
#define _unlikely(x)	__builtin_expect(!!(x), 0)

/**
 * @macro _force_inline
 * @brief inline directive for gcc-compatible compilers
 */
#define _force_inline	inline
// #define _force_inline

/**
 * @struct psort_occ_s
 */
struct psort_occ_s {
	uint64_t occ[WCR_OCC_SIZE];
};

/**
 * @struct psort_buffer_counter_s
 */
struct psort_buffer_counter_s {
	uint8_t cnt[WCR_OCC_SIZE];
};

/**
 * @struct psort_buffer_s
 */
struct psort_buffer_s {
	uint8_t buf[WCR_OCC_SIZE][WCR_BUF_SIZE];
};

/**
 * @struct psort_thread_context_s
 */
struct psort_thread_context_s {
	struct psort_occ_s *occ;
	struct psort_buffer_counter_s *cnt;
	struct psort_buffer_s *buf;
	int32_t digit;
	int32_t num_threads;
	void *src;
	void *dst;
	int64_t from, to;
};

/**
 * @fn aligned_malloc
 *
 * @brief an wrapper of posix_memalign function
 */
static _force_inline
void *aligned_malloc(size_t size, size_t align)
{
	void *ptr;
	posix_memalign(&ptr, align, size);
	return(ptr);
}

/**
 * @fn psort_dispatcher
 */
static
void *psort_dispatcher(
	void *arg,
	void *item)
{
	debug("arg(%p), item(%p)", arg, item);
	((void (*)(struct psort_thread_context_s *))item)(
		(struct psort_thread_context_s *)arg);
	return(NULL);
}

/* instanciate radixsort and quicksort */
#define _join(a, b)			a##b
#define join(a, b)			_join(a, b)

#undef UNITTEST_UNIQUE_ID

/* 16bit */
#define elem_t				uint16_t
#define SUFFIX				16
#define rd 					_rd
#define wr 					_wr
#define ex 					_ex
#define p 					_p
#define e 					_e
#define UNITTEST_UNIQUE_ID	SUFFIX
#include "psort_radix_intl.c"
// #include "psort_quick_intl.c"

/* 32bit */
#define elem_t				uint32_t
#define SUFFIX				32
#define rd 					_rd
#define wr 					_wr
#define ex 					_ex
#define p 					_p
#define e 					_e
#define UNITTEST_UNIQUE_ID	SUFFIX
#include "psort_radix_intl.c"
// #include "psort_quick_intl.c"

/* 64bit */
#define elem_t				uint64_t
#define SUFFIX				64
#define rd 					_rd
#define wr 					_wr
#define ex 					_ex
#define p 					_p
#define e 					_e
#define UNITTEST_UNIQUE_ID	SUFFIX
#include "psort_radix_intl.c"
// #include "psort_quick_intl.c"

/* 128bit */
#define elem_t				elem_128_t
#define SUFFIX				128
#define rd 					rd_128
#define wr 					wr_128
#define ex 					ex_128
#define p 					p_128
#define e 					e_128
#define UNITTEST_UNIQUE_ID	SUFFIX
#include "psort_radix_intl.c"
// #include "psort_quick_intl.c"

/**
 * @fn psort_full
 */
int psort_full(
	void *arr,
	int64_t len,
	int64_t elem_size,
	int64_t num_threads)
{
	switch(elem_size) {
		case 2: psort_partialsort_parallel_16(arr, len, num_threads, 0, 2); return(0);
		case 4: psort_partialsort_parallel_32(arr, len, num_threads, 0, 4); return(0);
		case 8: psort_partialsort_parallel_64(arr, len, num_threads, 0, 8); return(0);
		case 16: psort_partialsort_parallel_128(arr, len, num_threads, 0, 16); return(0);
		default: return(-1);
	}
	return(-1);
}

/**
 * @fn psort_half
 */
int psort_half(
	void *arr,
	int64_t len,
	int64_t elem_size,
	int64_t num_threads)
{
	switch(elem_size) {
		case 2: psort_partialsort_parallel_16(arr, len, num_threads, 0, 1); return(0);
		case 4: psort_partialsort_parallel_32(arr, len, num_threads, 0, 2); return(0);
		case 8: psort_partialsort_parallel_64(arr, len, num_threads, 0, 4); return(0);
		case 16: psort_partialsort_parallel_128(arr, len, num_threads, 0, 8); return(0);
		default: return(-1);
	}
	return(-1);
}

/**
 * @fn psort_partial
 */
int psort_partial(
	void *arr,
	int64_t len,
	int64_t elem_size,
	int64_t num_threads,
	int64_t from,
	int64_t to)
{
	switch(elem_size) {
		case 2: psort_partialsort_parallel_16(arr, len, num_threads, from, to); return(0);
		case 4: psort_partialsort_parallel_32(arr, len, num_threads, from, to); return(0);
		case 8: psort_partialsort_parallel_64(arr, len, num_threads, from, to); return(0);
		case 16: psort_partialsort_parallel_128(arr, len, num_threads, from, to); return(0);
		default: return(-1);
	}
	return(-1);
}

/**
 * end of psort.c
 */