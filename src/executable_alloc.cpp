#include "executable_alloc.h"

#include <iostream>
#include <cassert>
#include <vector>

#include <Windows.h>

#if 0
void* alloc_executable(size_t size, PageAccess access, OnAlloc on_alloc)
{
  assert(on_alloc);
  SYSTEM_INFO system_info;
  GetSystemInfo(&system_info);
  auto const page_size = system_info.dwPageSize;

  // prepare the memory in which the machine code will be put (it's not executable yet):
  auto const buffer = VirtualAlloc(nullptr, page_size, MEM_COMMIT, PAGE_READWRITE);


  // copy the machine code into that memory:
  on_alloc(buffer, size);



  // mark the memory as executable:
  DWORD dummy;
  VirtualProtect(buffer, size, PAGE_EXECUTE_READ, &dummy);


  // free the executable memory:
  VirtualFree(buffer, 0, MEM_RELEASE);

  // use your std::int32_t:
  std::cout << result << "\n";
	return NULL;
}

void on_alloc(void* buffer, int size)
{
  std::vector<unsigned char> const code =
  {
      0xb8,                   // move the following value to EAX:
      0x05, 0x00, 0x00, 0x00, // 5
      0xc3                    // return what's currently in EAX
  };
  std::memcpy(buffer, code.data(), code.size());
}

void test_alloc_executable()
{

  auto buffer = alloc_executable()
  // interpret the beginning of the (now) executable memory as the entry
  // point of a function taking no arguments and returning a 4-byte int:
  auto const function_ptr = reinterpret_cast<std::int32_t(*)()>(buffer);

  // call the function and store the result in a local std::int32_t object:
  auto const result = function_ptr();
}
#endif