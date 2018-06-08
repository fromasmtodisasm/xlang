#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "exp.h"
#include "interpreter.h"

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
	if ((program = fopen(name, "r")) != NULL)
	{
		int len = 0;
		int real_len = 0;
		int lines = 0;
		fseek(program, 0, SEEK_END);
		fseek(program, 0, SEEK_SET);
		while (1)
		{
			int cur_len = getline(program);
			if (cur_len)
			{
				len += cur_len;
				
			}
			else
			{
				break;
			}
		}
		fseek(program, 0, SEEK_SET);
 		source = (char*)malloc(len);
		fread(source, 1, len, program);
		source[len] = '\0';
		fclose(program);
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
	char *buffer = "a=b;";
	char buf[255];
	char *expression = buffer;
	FILE *test;

	if (argc > 1)
	{
		expression = buf;
		int cur_file = 1;
		for (; cur_file < argc; cur_file++)
		{
			if (buffer = loadProgram(argv[cur_file]))
			{
				expr(&buffer);
			}
			
		}
	}
	else
	{
		char *buffer = malloc(256);
		usage(basename(argv[0]));
		while (fgets(buffer, 255, stdin) != NULL)
		{
			if (expr(&buffer) == -1)
				break;
		}
	}
	getchar();
	return 0;
}
