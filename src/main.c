#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "exp.h"
#include "interpreter.h"
#include "preprocessor.h"
#include "fileutils.h"

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

int main(int argc, char **argv)
{
	char *source = NULL;
	char buf[255];
	char *expression = source;
	FILE *test;

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
				start(&source);
			}
			
		}
	}
	else
  {
    int buffer_size = 1024;
    source = malloc(buffer_size);
    usage(basename(argv[0]));
    xlang_context* ctx = xlang_create();
    if (ctx != NULL)
    {
      while (fgets(source, buffer_size, stdin) != NULL)
      {
        xlang_set_buffer(ctx, source);
        if (xlang_parse(ctx) == false)
          break;
      }
    }

  }
	return 0;
}
