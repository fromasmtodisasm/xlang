#include "lexer.h"
#include "common.h"
#include <ctype.h>
#include <string.h>

/******** Static variables *********/
#define CURTOK() (curr_context->curr_token)
#define NUMTOKS 2

/******** Static variables *********/
static token_t *token[2] = { NULL,NULL };
static char *source;

/******** Global variables *********/
context_t context;
context_t *curr_context;
struct commands { /* таблица зарезервированных слов */
	char command[20];
	token_type tok;
} table[] = { /* В эту таблицу */
	"if", lcIF, /* команды должны быть введены на нижнем регистре. */
	"else", lcELSE,
	"for", lcFOR,
	"do", lcDO,
	"while", lcWHILE,
	"char", lcCHAR,
	"int", lcINT,
	"return", lcRETURN,
	"quit", lcABORT,
	"print", lcPRINT,
	"read", lcREAD,
	"break", lcBREAK,
	"begin", lcBEGINBLOCK,
	"function", lcFUNCTION,
	"interprete", lcINTERPRETE,
	"end", lcENDBLOCK,
	"", lcEND  /* конец таблицы */
};

char *token_to_string[] =
{
	"lcEND",
	"lcDELIM",
	"lcSEMI",
	"lcCOMMENT",
	"lcKEYWORD",
	"lcIDENT",
	"lcNUMBER",
	"lcSTRING",
	"lcPLUS",
	"lcMINUS",
	"lcMUL",
	"lcDIV",
	"lcASSIGN",
	"lcPLUS_ASSIGN",
	"lcMINUS_ASSIGN",
	"lcMUL_ASSIGN",
	"lcDIV_ASSIGN",
	"lcEQ",
	"lcLBRACE",
	"lcRBRACE",
	"lcBREAK"
	"lcIF",
	"lcELSE",
	"lcFOR",
	"lcDO",
	"lcWHILE",
	"lcCHAR",
	"lcINT",
	"lcRETURN",
	"lcLBRACKET",
	"lcRBRACKET",
	"lcABORT",
	"lcPRINT",
	"lcUNKNOWN"
};
token_type is_keyword(char *name)
{
	token_type type = lcEND;
	for (int i = 0; table[i].tok != lcEND; i++)
	{
		if (!strcmp(name, table[i].command))
		{
			type = table[i].tok;
			break;
		}
	}
	return type;
}
int lexerInit(char *src)
{
	token[0] = (token_t*)malloc(sizeof(token_t));
	token[1] = (token_t*)malloc(sizeof(token_t));
	context_t *prev = (context_t*)malloc(sizeof(context_t));
	context_t *next = (context_t*)malloc(sizeof(context_t));

	/****************************/
	context.prev = prev;
	context.next = next;
	/****************************/ 
	prev->next = &context;
	prev->prev = next;
	/***************************/
	next->prev = &context;
	next->next = prev;
	/****************************/
	curr_context = &context;

	curr_context->pos = src;
	curr_context->cur_line = 0;
	
	return 1;
}

int is_hexdigit(int digit)
{

}

char *get_pos()
{
	return curr_context->curr_token.pos;
}

void set_pos(char *pos)
{
	curr_context->curr_token.pos = pos;
}

token_t* get_token(token_direct direction)
{
	char *pos = curr_context->pos;
	token_type type = lcEND;
	void *value = NULL;
	char *begin = pos;


	if (direction == PREV)
	{
		curr_context = curr_context->prev;
	}
	else if (direction == NEXT)
	{
		curr_context = curr_context->next;
		while (
			*pos == ' ' ||
			*pos == '\t' ||
			*pos == '\n')
		{
			if (*pos == '\n')
			{
				curr_context->cur_line++;
			}
			pos++;
		}
		begin = pos;
		while (*pos == '/' && (*(pos + 1) == '/' || *(pos + 1) == '*'))
		{
			if (*(pos + 1) == '/')
			{
				pos += 2;
				while (*pos)
				{
					if (*pos == '\n')
					{
						type = lcCOMMENT;
						break;
					}
					pos++;
				}
			}
			else if (*(pos + 1) == '*')
			{
				pos += 2;
				while (*pos)
				{
					if (*pos == '*' && *(pos + 1) == '/')
					{
						type = lcCOMMENT;
						pos += 2;
						break;
					}
					pos++;
				}
			}
			while (
				*pos == ' ' ||
				*pos == '\t' ||
				*pos == '\n')pos++;
			/*if (type == lcEND)
				type = lcUNKNOWN;
*/
		}
		if (isalpha(*pos) || *pos == '_')
		{
			int len = 0;
			while (isalpha(*pos) || *pos == '_' || isdigit(*pos))
			{
				len++;
				pos++;
			}
			value = malloc(len + 1);
			memcpy(value, pos - len, len);
			((char*)(value))[len] = '\0';
			int tmp;
			if ((tmp = is_keyword(value)) != lcEND)
			{
				type = tmp;
			}
			else
			{
				type = lcIDENT;
			}

		}
		else if (isdigit(*pos))
		{
			int val = 0;
			int hex_val = 0;
			int radix = 10;
			if (*pos == '0')
			{
				if (*(pos + 1) == 'x' || *(pos + 1) == 'X')
				{
					radix = 16;
					pos += 2;
					while (*pos)
					{
						if (*pos >= '0' && *pos <= '9')
						{
							val = val * radix + (*pos - '0');
							pos++;
						}
						else if (*pos >= 'A' && *pos <= 'F'
							||
							*pos >= 'a' && *pos <= 'f')
						{
							if (*pos >= 'a')
							{
								hex_val = *pos - 'A' - 32 + 10;
							}
							else
							{
								hex_val = *pos - 'A' + 10;
							}
							val = val * radix + (hex_val);
							pos++;
						}
						else
						{
							break;
						}
					}
				}
				else
				{
					radix = 8;
				}
			}
			else
			{
				while (*pos)
				{
					if (isdigit(*pos))
					{
						val = val * 10 + (*pos - '0');
						pos++;
					}
					else
					{
						break;
					}
				}
			}
			value = (void*)val;
			type = lcNUMBER;
		}
		else if (*pos == ';')
		{
			type = lcSEMI;
			pos++;
		}
		else if (*pos == '(')
		{
			type = lcLBRACE;
			pos++;
		}
		else if (*pos == ')')
		{
			type = lcRBRACE;
			pos++;
		}
		else if (*pos == '{')
		{
			type = lcLBRACKET;
			pos++;
		}
		else if (*pos == '}')
		{
			type = lcRBRACKET;
			pos++;
		}
		else if (*pos == '\"')
		{
			int len = 0;
			pos++;
			while (*pos)
			{
				if (*pos == '\"')
				{
					pos++;
					break;
				}
				len++;
				pos++;
			}
			
			value = malloc(len + 1);
			memcpy(value, pos - len - 1, len);
			((char*)(value))[len] = '\0';
			type = lcSTRING;
		}
		else if (type == lcEND)
		{
			switch (*pos)
			{
			case '+':
			{
				if (pos[1] == '=')
				{
					pos++;
					type = lcPLUS_ASSIGN;
				}
				else
				{
					type = lcPLUS;
				}
				break;
			}
			case '-':
			{
				if (pos[1] == '=')
				{
					type = lcMINUS_ASSIGN;
					pos++;
				}
				else
				{
					type = lcMINUS;
				}
				break;
			}
			case '*':
			{
				if (pos[1] == '=')
				{
					pos++;
					type = lcMUL_ASSIGN;
				}
				else
				{
					type = lcMUL;
				}
				break;
			}
			case '/':
			{
				if (pos[1] == '=')
				{
					pos++;
					type = lcDIV_ASSIGN;
				}
				else
				{
					type = lcDIV;
				}
				break;
			}
			case '=':
			{
				if (pos[1] == '=')
				{
					type = lcEQ_OP;
					pos++;
				}
				else
				{
					type = lcASSIGN;
				}
				break;
			}
			case '<':
			{
				if (pos[1] == '=')
				{
					type = lcLE_OP;
					pos++;
				}
				else
				{
					type = lcL_OP;
				}
				break;
			}
			case '>':
			{
				if (pos[1] == '=')
				{
					type = lcGE_OP;
					pos++;
				}
				else
				{
					type = lcG_OP;
				}
				break;
			}
			case '!':
			{
				if (pos[1] == '=')
				{
					type = lcNE_OP;
					pos++;
				}
				break;
			}
			}
			if (type != lcEND)
				pos++;
		}
		else if (*pos == NULL)
		{
			type = lcEND;
		}
		else
		{
			type = lcUNKNOWN;
			pos++;
		}
		CURTOK().type = type;
		CURTOK().text = value;
		CURTOK().pos = begin;
		curr_context->pos = pos;
	}
	
	//printf("\ncurr_tok = %s\n", token_to_string[CURTOK().type - BASE_INDEX]);
	return &CURTOK();
}
