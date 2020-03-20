#pragma once

#ifdef __cplusplus
#define ENUMCLASS enum class
#else
#define ENUMCLASS enum
#endif

ENUMCLASS PageAccess{
	Execute = 0x10,
	Execute_Read = 0x20,
	Execute_ReadWrite = 0x40
};

typedef void (OnAlloc)(void* buffer, int size);

void* alloc_executable(size_t size, PageAccess access, OnAlloc on_alloc);
