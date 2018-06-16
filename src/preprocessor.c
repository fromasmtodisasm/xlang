#include "preprocessor.h"

FILE *preprocess(char* name)
{
	FILE *processed;

	processed = fopen(name, "rb");

	return processed;
}
