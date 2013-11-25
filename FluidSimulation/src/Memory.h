/* TODO: This only works for Win32 */

#ifdef RC_WIN32

#include <stdlib.h>
#include <crtdbg.h>

/*
 * Memory debugger
 */

/* DON'T touch unless you know exactly what you're doing! */

#ifndef RC_MEMORY_H
#define RC_MEMORY_H

#define MEM_METHOD_MALLOC_FREE			0
#define MEM_METHOD_NEW_DELETE			1
#define MEM_METHOD_NEW_DELETE_ARRAY		2

/* Memory declarations */

#ifndef RC_MEMORY_DEBUG

		/* Release declarations */
	void *MemoryAllocate(
			u32		mem_size,
			u32		method
		);

	void *MemoryAllocateSetZero(
			u32		n,
			u32		type_size,
			u32		method
		);

	void MemoryFree(
			void		*ptr,
			u32			method
		);

	void *MemoryReallocate(
			void	*old_ptr,
			u32		new_size,
			u32		method
		);

#endif /* !RC_MEMORY_DEBUG */

	/* Release/Debug declarations */
void *operator new(
		size_t		mem_size
	);

void *operator new [](
		size_t		mem_size
	);

void operator delete(
		void *ptr
	);

void operator delete [](
		void *ptr
	);

#ifdef RC_MEMORY_DEBUG

		/* Debug declarations */
	void *MemoryAllocateDebug(
			u32			mem_size,
			u32			method,
			const char	*file,
			const char	*func,
			int			line
		);

	void *MemoryAllocateSetZeroDebug(
			u32			n,
			u32			type_size,
			u32			method,
			const char	*file,
			const char	*func,
			int			line
		);

	void MemoryFreeDebug(
			void		*ptr,
			u32			method
		);

	void *MemoryReallocateDebug(
			void		*old_ptr,
			u32			new_size,
			u32			method,
			const char	*file,
			const char	*func,
			int			line
		);

	i32 MemorySetInfo(
			const char	*new_file,
			const char	*new_func,
			i32			new_line
		);

#endif /* RC_MEMORY_DEBUG */

#ifdef malloc
	#undef malloc
#endif

#ifdef calloc
	#undef calloc
#endif

#ifdef free
	#undef free
#endif

#ifdef realloc
	#undef realloc
#endif

#ifdef new
	#undef new
#endif

#ifdef delete
	#undef delete
#endif


#ifdef RC_MEMORY_DEBUG

	#define malloc(mem_size)							MemoryAllocateDebug(mem_size, MEM_METHOD_MALLOC_FREE, __FILE__, __FUNCTION__, __LINE__)
	#define calloc(n, type_size)						MemoryAllocateSetZeroDebug(n, type_size, MEM_METHOD_MALLOC_FREE, __FILE__, __FUNCTION__, __LINE__)
	#define free(ptr)									MemoryFreeDebug(ptr, MEM_METHOD_MALLOC_FREE)
	#define realloc(old_ptr, new_size)					MemoryReallocateDebug(old_ptr, new_size, MEM_METHOD_MALLOC_FREE, __FILE__, __FUNCTION__, __LINE__)

	#define new											(MemorySetInfo(__FILE__, __FUNCTION__, __LINE__)) ? 0 : new 

#else /* RC_MEMORY_DEBUG */

	#define malloc(mem_size)							MemoryAllocate(mem_size, MEM_METHOD_MALLOC_FREE)
	#define calloc(n, mem_size)							MemoryAllocateSetZero(n, type_size, MEM_METHOD_MALLOC_FREE)
	#define free(ptr)									MemoryFree(ptr, MEM_METHOD_MALLOC_FREE)
	#define realloc(old_ptr, new_size)					MemoryReallocate(old_ptr, new_size, MEM_METHOD_MALLOC_FREE)

#endif /* RC_MEMORY_DEBUG */



/** Get total number of allocated bytes */
u32 getAllocationSize();

/** Get the peak memory usage through out the runtime */
u32 getAllocationPeak();

/* LOWPRIO: Not yet implemented */
/** Get the peak memory since the last "local peak reset" */
u32 getAllocationPeakLocal();

/** Get the number of active allocations */
u32 getCurrentAllocationCount();

/** Get the total number of allocations made from start of execution */
u32 getAllocationCount();

/** Get the total number of deallocations since start of execution */
u32 getDeallocationCount();

/** Get the total number of reallocations since start of execution */
u32 getReallocationCount();

/** Log current allocations to "mem.txt" */
void logAllocations();

/**
 * Ensure that there are no memory corruptions
 * Note: This is not fool proof and some corruptions might still go undetected.
 */
void ensure_heap();

/* Register commands */
void MemoryRegisterCommands();

#endif /* RC_MEMORY_H */

#else

	#ifndef RC_MEMORY_H
	#define RC_MEMORY_H

		u32 getAllocationSize();

		u32 getAllocationPeak();

		u32 getAllocationPeakLocal();

		u32 getCurrentAllocationCount();

		u32 getAllocationCount();

		u32 getDeallocationCount();

		u32 getReallocationCount();

		void logAllocations();

		void ensure_heap();

		void MemoryRegisterCommands();

	#endif

#endif

