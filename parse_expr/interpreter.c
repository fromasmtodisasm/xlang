#include "interpreter.h"
#include "lexer.h"
#include "exp.h"
#include "common.h"

#include <stdio.h>

extern char *token_to_string[];
void skip_compound_statement();
void skip_statement();
void eat_tokens(token_type skip_to);

void exptected_func(char *exptected)
{
	printf("Error. Expected %s\n", exptected);
}

int skip_if()
{
	eat_tokens(lcRBRACE);
	if (get_token(NEXT)->type == lcLBRACKET)
	{
		skip_compound_statement();
	}
	else
	{
		skip_statement();
	}
}

void eat_tokens(token_type skip_to)
{
	token_type type = lcEND;
	if (get_token(CURR)->type == skip_to)
		return;
	while (((type = get_token(NEXT)->type) != skip_to) && type != lcEND );
}
void skip_statement()
{
	int stop = 0;
	while (!stop)
	{
		switch (get_token(CURR)->type)
		{
		case lcLBRACKET:
		{
			skip_compound_statement();
			if (get_token(CURR)->type != lcRBRACKET)
			{
				printf("error: expected }\n");
			}
			/*else
			{

			}*/
		}
		break;
		case lcIF:
		{
			skip_if();
		}
		break;
		case lcRBRACKET:
			return;
		default:
			eat_tokens(lcSEMI);
			if (get_token(CURR)->type == lcSEMI)
				break;
			else
				return;
		}
		get_token(NEXT);
	}

}

void skip_compound_statement()
{
	int bracket_lvl = 0;
	int res = 0;
	if (!(get_token(CURR)->type == lcLBRACKET && get_token(NEXT)->type == lcRBRACKET) && (get_token(CURR) != lcEND))
	{
		if (get_token(PREV)->type == lcLBRACKET)
		{
			get_token(NEXT);
			skip_statement();
			if (get_token(CURR)->type == lcRBRACKET)
			{

			}
			else
			{
				printf("error: expected }\n");
			}
		}
	}
}

int do_if()
{
	token_t *token = get_token(NEXT);
	int condition = 0;
	if (token->type == lcLBRACE)
	{
		get_token(NEXT);
		condition = assignment_expression();
		if ((token = get_token(CURR))->type == lcRBRACE)
		{
			if ((token = get_token(NEXT))->type == lcLBRACKET)
			{
				if (!condition)
				{
					get_token(NEXT);
					skip_compound_statement();
					if (get_token(NEXT)->type == lcELSE)
					{
						if (get_token(NEXT)->type == lcLBRACKET)
						{
							compound_statement();
							if ((token = get_token(CURR))->type != lcRBRACKET)
							{
								exptected_func("RBRACKET");
								//exit(-1);
							}
						}
						else
						{
							statement();
						}
					}
				}
				else
				{
					compound_statement();
					if ((token = get_token(CURR))->type != lcRBRACKET)
					{
						exptected_func("RBRACKET");
						//exit(-1);
					}
					if (get_token(NEXT)->type == lcELSE)
					{
						get_token(NEXT);
						skip_statement();
					}
					else
					{
						get_token(PREV);
					}
				}
			}
			else
			if (condition)
			{
				statement();
				skip_statement();
				if (get_token(NEXT)->type == lcELSE)
				{
					get_token(NEXT);
					skip_compound_statement();
				}
				else
				{
					get_token(PREV);
				}
			}
			else
			{
				get_token(NEXT);
				if (get_token(CURR)->type == lcELSE)
				{
					if ((token = get_token(NEXT))->type == lcLBRACKET)
					{
						compound_statement();
						if ((token = get_token(CURR))->type != lcRBRACKET)
						{
							exptected_func("RBRACKET");
						}
					}
					else
					{
						statement();
					}
				}
			}
		}
	}
}

int do_while()
{
	token_t *token = get_token(NEXT);
	int condition = 0;
	
	if (token->type == lcLBRACE)
	{
		char *pos_begin = get_pos();
		char* pos_end = pos_begin;
		
		while (get_token(NEXT), condition = assignment_expression())
		{	
			if ((token = get_token(CURR))->type == lcRBRACE)
			{
				if (get_token(NEXT)->type == lcLBRACKET)
				{
					compound_statement();
					if ((token = get_token(CURR))->type != lcRBRACKET)
					{
						exptected_func("RBRACKET");
						//exit(-1);
					}
				}
				else
				{
					statement();
				}
				pos_end = get_pos();
				set_pos(pos_begin);
				get_token(NEXT);
			}
		}
		set_pos(pos_end);
		get_token(NEXT);
		skip_compound_statement();
	}

	get_token(NEXT);
	return 0;
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
	int retval = 0;

	parser_init();

	if ((lexerInit(*buffer)) != NULL)
	{
		do
		{
			get_token(NEXT);
			retval = compound_statement();
		} while (get_token(NEXT)->type != lcEND);	
	}

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
			expr_val = assignment_expression();
			printf("%d", expr_val);
		}
	} while ((token = get_token(CURR))->type == lcSTRING || token->type == lcIDENT);
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

int statement()
{
	int res = 0;
	int expr_len;
	token_t *token;
	int retval = 0;
	int stop = 0;

	while (!stop)
	{
		switch (get_token(CURR)->type)
		{
		case lcIF:
		{
			do_if();
		}
		break;
		case lcWHILE:
		{
			do_while();
		}
		break;
		case lcLBRACKET:
		{
			res = compound_statement();
			if (get_token(CURR)->type != lcRBRACKET)
			{
				printf("error: expected }\n");
			}
			/*else
			{
				
			}*/
		}
		break;
		case lcBREAK:
		{
			token = get_token(NEXT);
			puts("break point");
			getchar();
		}
		break;
		case lcFUNCTION:
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
		break;
		case lcPRINT:
		{
			print();
			if ((token = get_token(CURR))->type == lcSEMI)
			{
				get_token(NEXT);
			}
		}
		break;
		case lcREAD:
		{
			read();

		}
		break;
		case lcABORT:
		{
			get_token(NEXT);
			puts("This is abort!");
			retval = -1;
			goto abort;
		}
		break;
		case lcINTERPRETE:
		{
			interprete();
		}
		break;
		case lcIDENT:
		{

			res = assignment_expression();
			if (get_token(CURR)->type != lcSEMI)
			{
				exptected_func("SEMI");
				goto abort;
			}
			get_token(NEXT);
		}
		break;
		default:
			stop = 1;
			break;
		}		
	}

abort:
	return retval;

}
int compound_statement()
{
	int res = 0;
	int expr_len;
	token_t *token;
	int retval = 0;

	if (get_token(CURR)->type == lcLBRACKET && get_token(NEXT)->type == lcRBRACKET)
	{
		retval = 0;
	}
	else
	{
		if (get_token(PREV)->type == lcLBRACKET)
		{
			get_token(NEXT);
			res = statement();
			if (get_token(CURR)->type == lcRBRACKET)
			{

			}
			else
			{
				printf("error: expected }\n");
			}
		}
		
		
	}
	return res;
}