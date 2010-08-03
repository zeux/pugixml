#include "allocator.hpp"

#include <string.h>

// Low-level allocation functions
#if defined(_WIN32) || defined(_WIN64)
#	ifdef __MWERKS__
#		pragma ANSI_strict off // disable ANSI strictness to include windows.h
#		pragma cpp_extensions on // enable some extensions to include windows.h
#	endif

#	ifdef _XBOX_VER
#		define NOD3D
#		include <xtl.h>
#	else
#		include <windows.h>
#	endif

namespace
{
	const size_t PAGE_SIZE = 4096;

    size_t align_to_page(size_t value)
    {
        return (value + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    }

	void* allocate_page_aligned(size_t size)
    {
        // We can't use VirtualAlloc because it has 64Kb granularity so we run out of address space quickly
        void* result = malloc(size + PAGE_SIZE);

        return (void*)align_to_page((size_t)result);
    }

	void* allocate(size_t size)
	{
		size_t aligned_size = align_to_page(size);

		void* ptr = allocate_page_aligned(aligned_size + PAGE_SIZE);
		if (!ptr) return 0;

		void* end = (char*)ptr + aligned_size;

		DWORD old_flags;
		VirtualProtect(end, PAGE_SIZE, PAGE_NOACCESS, &old_flags);

		return (char*)end - size;
	}

	void deallocate(void* ptr, size_t size)
	{
		size_t aligned_size = align_to_page(size);

		void* rptr = (char*)ptr + size - aligned_size;

        DWORD old_flags;
        VirtualProtect(rptr, aligned_size + PAGE_SIZE, PAGE_NOACCESS, &old_flags);
	}
}
#else
#	include <stdlib.h>

namespace
{
	void* allocate(size_t size)
	{
		return malloc(size);
	}

	void deallocate(void* ptr, size_t size)
	{
		(void)size;

		free(ptr);
	}
}
#endif

// High-level allocation functions
void* memory_allocate(size_t size)
{
	void* result = allocate(size + sizeof(size_t));
	if (!result) return 0;

	memcpy(result, &size, sizeof(size_t));

	return (size_t*)result + 1;
}

size_t memory_size(void* ptr)
{
	size_t result;
	memcpy(&result, (size_t*)ptr - 1, sizeof(size_t));

	return result;
}

void memory_deallocate(void* ptr)
{
	if (!ptr) return;

	size_t size = memory_size(ptr);

	deallocate((size_t*)ptr - 1, size + sizeof(size_t));
}

