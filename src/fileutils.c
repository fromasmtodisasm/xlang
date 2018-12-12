#include "fileutils.h"
#include <malloc.h>


char *file2str(FILE *file)
{
	char *str = NULL;
	int len = 0;
	int real_len = 0;
	int lines = 0;

	fseek(file, 0, SEEK_END);
	len = ftell(file);
	fseek(file, 0, SEEK_SET);
	str = (char*)malloc(len);
	fread(str, 1, len, file);
	str[len] = '\0';
	fclose(file);

	return str;
}
