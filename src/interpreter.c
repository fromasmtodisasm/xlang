#include "interpreter.h"
#include "lexer.h"
#include "exp.h"
#include "common.h"

#include <stdio.h>

extern char *token_to_string[];
void skip_compound_statement();
void skip_statement();
int function_definition();
int declaration_list();

token_type eat_tokens(token_type skip_to);

void exptected_func(char *exptected)
{
	printf("Error. Expected %s\n", exptected);
}

void skip_if()
{
	if (eat_tokens(lcRBRACE) == lcRBRACE)
	{
		get_token(NEXT);
		skip_statement();
	}
}

void skip_while()
{
	if (eat_tokens(lcRBRACE) == lcRBRACE)
	{
		get_token(NEXT);
		skip_statement();
	}
}

token_type eat_tokens(token_type skip_to)
{
	token_type type = lcEND;
	if ((type = get_token(CURR)->type) == skip_to)
		return type;
	while (((type = get_token(NEXT)->type) != skip_to) && type != lcEND);
	return type;
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
			return;
		}
		break;
		case lcIF:
		{
			skip_if();
		}
		break;
		case lcWHILE:
		{
			skip_while();
		}
		break;
		case lcRBRACKET:
			return;
		default:
			if (eat_tokens(lcSEMI) == lcSEMI)
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
	if (!(get_token(CURR)->type == lcLBRACKET && get_token(NEXT)->type == lcRBRACKET) && (get_token(CURR)->type != lcEND))
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

way_out do_if()
{
	token_t *token = get_token(NEXT);
	int condition = 0;
	way_out out = NORMAL;
	if (token->type == lcLBRACE)
	{
		get_token(NEXT);
		condition = assignment_expression();
		if ((token = get_token(CURR))->type == lcRBRACE)
		{
			if (condition)
			{
				get_token(NEXT);
				if ((out = statement(SELECTION)) == CONTINUE || out == BREAK)
				{
					//get_token(PREV);
				}
				else
				if (get_token(NEXT)->type == lcELSE)
				{
					get_token(NEXT);
					skip_statement();
				}
			}
			else
			{
				get_token(NEXT);
				skip_statement();
				if (get_token(NEXT)->type == lcELSE)
				{
					get_token(NEXT);
					out = statement(SELECTION);
				}
			}
		}
	}
	/*get_token(NEXT);*/
	return out;
}

int do_while()
{
	token_t *token = get_token(NEXT);
	int condition = 0;
	way_out out = NORMAL;
	
	if (token->type == lcLBRACE)
	{
		char *pos_begin = get_pos();
		char* pos_end = pos_begin;
		
		while (get_token(NEXT), condition = assignment_expression())
		{	
			if ((token = get_token(CURR))->type == lcRBRACE)
			{
				get_token(NEXT);
				if ((out = statement(ITERATION)) == BREAK)
				{
					break;
				}
				else if (out == CONTINUE || out == NORMAL)
				{
					pos_end = get_pos();
					set_pos(pos_begin);
					get_token(NEXT);
					continue;
				}
				else if (out == RETURN)
				{
					return RETURN;
				}
			}
		}
		get_token(NEXT);
		skip_statement();
	}

	get_token(NEXT);
	return out;
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

int start(char **buffer)
{
	int retval = 0;

	exp_parser_init();

	if ((lexerInit(*buffer)) != 0)
	{
		do
		{
			get_token(NEXT);
			retval = function_definition();
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

int my_printf()
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
		start((char**)&(get_token(CURR)->text));
	}
	get_token(NEXT);
	return 0;
}

way_out statement(compound_origin origin)
{
	int res = 0;
	int expr_len;
	token_t *token;
	way_out out = NORMAL;
	int stop = 0;

	while (!stop)
	{
		switch (get_token(CURR)->type)
		{
		case lcIF:
		{
			if ((out = do_if()) == CONTINUE || out == BREAK)
				return out;
		}
		break;
		case lcWHILE:
		{
			do_while();
		}
		break;
		case lcBREAK:
		{
			if (get_token(NEXT)->type == lcSEMI)
			{
				get_token(NEXT);
				skip_statement();
				return BREAK;				
			}
			else
			{
				exptected_func("SEMI");
			}
		}
		break;
		case lcCONTINUE:
		{
			if (get_token(NEXT)->type == lcSEMI)
			{
				get_token(NEXT);
				skip_statement();
				return CONTINUE;
			}
			else
			{
				exptected_func("SEMI");
			}
		}
		break;
		case lcLBRACKET:
		{
			out = compound_statement(origin);
			if (get_token(CURR)->type != lcRBRACKET)
			{
				printf("error: expected }\n");
			}
			return out;
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
			out = -1;
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
	return out;

}
way_out compound_statement(compound_origin origin)
{
	way_out out = NORMAL;
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
			out = statement(origin);
			if (get_token(CURR)->type == lcRBRACKET)
			{

			}
			else
			{
				printf("error: expected }\n");
			}
		}
		
		
	}
	return out;
}

int is_type(token_type type)
{
	int res = FALSE;
	switch(type)
	{
		case lcINT:
		case lcVOID:
		case lcCHAR:
			res = TRUE;
			break;

	}
	return res;

}

int function_definition()
{
	way_out out;
	token_type type = get_token(CURR)->type;
	if (is_type(type) && get_token(NEXT)->type == lcIDENT)
	{
		get_token(NEXT);
		declaration_list();
		out = compound_statement(COMPOUND);
	}

	return 0;
}

int declaration_list()
{
	token_type type;
	int retval = -1;
	if (get_token(CURR)->type == lcLBRACE)
	{
		if (is_type(get_token(NEXT)->type))
		{
			if (get_token(NEXT)->type == lcIDENT)
			{
				while (get_token(NEXT)->type == lcCOMMA && get_token(NEXT)->type == lcINT && get_token(NEXT)->type == lcIDENT);
				if (get_token(CURR)->type != lcRBRACE)
				{
					exptected_func("RBRACE");
				}
				else
				{
					retval = 0;
				}
			}
		}
		else
		{
			if (get_token(CURR)->type != lcRBRACE)
			{
				exptected_func("RBRACE");
			}
			else
			{
				retval = 0;
			}

		}
	}
	get_token(NEXT);
}
