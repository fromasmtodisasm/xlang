#include "interpreter.h"
#include "lexer.h"
#include "exp.h"
#include "common.h"

#include <stdio.h>

extern char *token_to_string[];

void exptected_func(char *exptected)
{
	printf("Error. Expected %s\n", exptected);
}

int func_decl()
{
	char* funcname;
	token_t *token = get_token(CURR);
	
	funcname = token->text;
	if ((token = get_token(NEXT))->type == lcLBRACE)
	{
		if ((token = get_token(NEXT))->type == lcIDENT)
		{

		}
	}
	return 0;
}
int expr(char **buffer)
{
	int res = 0;
	char *expr = *buffer;
	int expr_len;
	token_t *token;
	int retval = 0;

	parser_init();

	if ((lexerInit(*buffer)) != NULL)
	{
		while ((token = get_token(NEXT))->type != lcEND)
		{
			/*
			if (token->type == lcINT || token->type == lcCHAR)
			{
				token = get_token(NEXT);
				if (token->type == lcIDENT)
				{

				}
			}
			else
			*/
			if (token->type == lcBREAK)
			{
				token = get_token(NEXT);
				puts("break point");
				getchar();
			}
			else if (token->type == lcFUNCTION)
			{
				if ((token = get_token(NEXT))->type == lcIDENT)
				{
					func_decl();
				}
				else
				{
					exptected_func("IDENT");
				}
			}
			else if (token->type == lcPRINT)
			{
				print();
			}
			else if (token->type == lcREAD)
			{
				read();
			}
			else if (token->type == lcABORT)
			{
				get_token(NEXT);
				puts("This is abort!");
				retval = -1;
				//getchar();
				goto abort;
			}
			else if (token->type == lcINTERPRETE)
			{
				interprete();
			}
			else
			{
				//get_token(NEXT);
				res = assign();
				if (/*get_token(CURR)->type != lcSEMI*/0)
				{
					exptected_func("SEMI");
					goto abort;
				}
			}
		}
	}
abort:

	return retval;
}

int is_print()
{
	token_t *token = get_token(CURR);
	int res = FALSE;
	if (token->type == lcPRINT)
	{
		res = TRUE;
	}
	else
	{
		get_token(PREV);
	}
	return res;
}

int print()
{
	token_t *token = get_token(NEXT);
	int stop = 0;
	
	do
	{
		char *number = "%d";
		char *string = "%s";
		char *curtype;
		int expr_val = 0;
		if (token->type == lcSTRING) {
			printf("%s", token->text);
			token = get_token(NEXT);
		}
		else {
			curtype = number;
			expr_val = assign();
			printf("%d", expr_val);
		}
	} while ((token = get_token(CURR))->type != lcSEMI && token->type != lcEND);
	puts("");
}

int read()
{
	token_t *token = get_token(CURR);
	int stop = 0;
	int tmp;
	while ((token = get_token(NEXT))->type != lcSEMI && token->type != lcEND)
	{
		if (token->type == lcIDENT)
		{
			scanf("%d", &tmp);
			assign_value(token->text, tmp);

		}
	} 
}

int interprete()
{
	if (get_token(NEXT)->type == lcSTRING)
	{
		expr(&(get_token(CURR)->text));
	}
	get_token(NEXT);
	return 0;
}