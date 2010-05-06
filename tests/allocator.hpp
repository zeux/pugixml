#pragma once

#include <stddef.h>

void* memory_allocate(size_t size);
size_t memory_size(void* ptr);
void memory_deallocate(void* ptr);
