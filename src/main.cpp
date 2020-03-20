#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#include "exp.h"
#include "interpreter.h"
#include "preprocessor.h"
#include "fileutils.h"
#ifdef __cplusplus
}
#endif // __cplusplus

#include <vector>
#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#if !defined _MSC_VER
	#define getline mygetline
#endif

int getline(FILE *fp)
{
	int ch = EOF;
	int len = 0;
	int lines = 0;
	while ((ch = getc(fp)) != EOF)
	{
		if (ch != '\n')
		{
			len++;
		}
		else
		{
			len++;
			break;
		}
	}
	return len;
}

char *loadProgram(char *name)
{
	FILE *program;
	char *source = NULL;
	if ((program = preprocess(name)) != NULL)
	{
		source = file2str(program);
	}
	return source;
}

void usage(char *prog_name)
{
	printf("Usage: %s file\n", prog_name);
}

char *basename(char *path)
{
	int pos = 0;
	for (int i = strlen(path) - 1; i > 0; i--)
	{
		if (path[i] == '\\')
		{
			break;
		}
		pos=i;
	}
	printf("%s\n", path+pos);
	return path + pos;
}

bool parse(xlang_context* ctx, char* source)
{
  xlang_set_buffer(ctx, source);
  return xlang_parse(ctx);
}

#if 0
bool preprocess(xlang_context* ctx, char* source)
{
  while
}
#endif

void test_alloc()
{
	std::vector<unsigned char> const code =
	{
			0xb8,                   // move the following value to EAX:
			0x9, 0x00, 0x00, 0x00, // 5
			0xc3                    // return what's currently in EAX
	};

	SYSTEM_INFO system_info;
	GetSystemInfo(&system_info);
	auto const page_size = system_info.dwPageSize;

	// prepare the memory in which the machine code will be put (it's not executable yet):
	auto const buffer = VirtualAlloc(nullptr, page_size, MEM_COMMIT, PAGE_READWRITE);

	// copy the machine code into that memory:
	std::memcpy(buffer, code.data(), code.size());

	// mark the memory as executable:
	DWORD dummy;
	VirtualProtect(buffer, code.size(), PAGE_EXECUTE_READ, &dummy);

	// interpret the beginning of the (now) executable memory as the entry
	// point of a function taking no arguments and returning a 4-byte int:
	auto const function_ptr = reinterpret_cast<std::int32_t(*)()>(buffer);

	// call the function and store the result in a local std::int32_t object:
	auto const result = function_ptr();

	// free the executable memory:
	VirtualFree(buffer, 0, MEM_RELEASE);

	// use your std::int32_t:
	std::cout << result << "\n";
}

int main(int argc, char **argv)
{
	char *source = NULL;
	char buf[255];
	char *expression = source;
	FILE *test;

  xlang_context* ctx = xlang_create();
  if (ctx != NULL)
  {
    if (argc > 1)
    {
      expression = buf;
      int cur_file = 1;
      for (; cur_file < argc; cur_file++)
      {
        printf("Load %s \n\n\n", argv[cur_file]);
        if (source = loadProgram(argv[cur_file]))
        {
          printf("source: \n%s\n\n", source);
          parse(ctx, source);
        }
        
      }
    }
    else
    {
      int buffer_size = 1024;
      source = (char*)malloc(buffer_size);
      usage(basename(argv[0]));
      while (fgets(source, buffer_size, stdin) != NULL)
      {
        if (parse(ctx, source) == false)
          break;
      }
    }
  }
	return 0;
}
