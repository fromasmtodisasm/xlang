#include "exp.h"
#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


variable *vars;



int make_builtin_vars()
{
	vars = malloc(sizeof(variable));
	static variable false =
	{
		"false",
		0
	};
	static variable true = {
		"true",
		1,
	};

	vars = &true;
	true.next = &false;
}

int parser_init()
{
	make_builtin_vars();
}

int block(char **buffer)
{
	return 0;
}

int lookup(char *name, int *val)
{
	variable *cur_var;
	int res = 0;
	for (cur_var = vars; cur_var != NULL; cur_var = cur_var->next)
	{
		if (cur_var->name)
			if (!strcmp(name, cur_var->name))
			{
				*val = cur_var->value;
				res = 1;
				break;
			}
	}
	return res;
}

int assign_value(char *name, int val)
{
	variable *cur_var;
	variable *tmp;
	for (cur_var = vars; cur_var != NULL; cur_var = cur_var->next)
	{
		tmp = cur_var;
		if (cur_var->name == NULL)
		{
			cur_var->name = name;
			cur_var->value = val;
			break;
		}
		else if (!strcmp(name, cur_var->name))
		{
			cur_var->name = name;
			cur_var->value = val;
			break;
		}
	}
	if (!cur_var)
	{
		tmp->next = malloc(sizeof(variable));
		tmp = tmp->next;
		tmp->name = name;
		tmp->value = val;
		tmp->next = NULL;

	}
}

int primary()
{
	int res = 0;

	token_t *token = get_token(CURR);
	token_direct direct = NEXT;
	switch (token->type)
	{

	case lcNUMBER:
	{
		res = token->text;
		break;
	}
	case lcIDENT:
	{
		if ((lookup(token->text, &res)) == NULL)
		{
			printf("Error, undefined variable <%s>\n", token->text);

			//exit(-1);
		}
		break;
	}
	case lcLBRACE:
	{
		token = get_token(NEXT);
		res = assign();
		token = get_token(CURR);
		if (token->type != lcRBRACE)
		{
			printf("Error, expected ')'\n");
			getchar();
			exit(-1);
		}
		break;
	}
	default:
		printf("Error, expected primary!!!\n");
		getchar();
		exit(-1);
	}
	get_token(direct);
	return res;
}

int factor()
{
	int res = 0;
	int stop = 0;
	token_t *token;

	res = primary();
	token = get_token(CURR);
	while ( token->type == lcMUL || token->type == lcDIV)
	{
		switch (token->type)
		{
		case lcMUL:
		{
			token = get_token(NEXT);
			res *= primary();
			break;
		}
		case lcDIV:
		{
			token = get_token(NEXT);
			res /= primary();
			break;
		}
		default:
			stop = 1;
			break;
		}
		token = get_token(CURR);
	}
	return res;
}

int calc_expr()
{
	char **buffer;
	token_t *token;
	int res = 0;
	int stop = 0;
	
	res = factor();
	token = get_token(CURR);
	while (token->type == lcPLUS || token->type == lcMINUS)
	{
		switch (token->type)
		{
		case lcPLUS:
		{
			token = get_token(NEXT);
			res += factor();
			break;
		}
		case lcMINUS:
		{
			token = get_token(NEXT);
			res -= factor();
			break;
		}
		default:
			stop = 1;
			break;
		}
		token = get_token(CURR);
	}
	return res;
}

int is_relop(token_type type)
{
	return 
		(
			lcAND_OP == type ||
			lcOR_OP == type ||
			lcLE_OP == type ||
			lcGE_OP == type ||
			lcEQ_OP == type ||
			lcNE_OP == type
		);
}

int relation()
{
	token_t *token = get_token(CURR);
	int res = 0;
	token_type type = token->type;
	res = calc_expr();

	type = (token = get_token(CURR))->type;
	
	while (is_relop(type))
	{
		token = get_token(NEXT);
		switch (type)
		{	

		case lcAND_OP:
			res &= calc_expr();
			break;
		case lcOR_OP:
			res |= calc_expr();
			break;
		case lcEQ_OP:
			res = res == calc_expr();
			break;
		case lcL_OP:
			res  = res < calc_expr();
		case lcG_OP:
			res = res > calc_expr();
			break;
		case lcLE_OP:
			res = res <= calc_expr();
		case lcGE_OP:
			res = res >= calc_expr();
			break;
		case lcN_OP:
			res != calc_expr();
			break;
		default:
			break;
		}
		type = (token = get_token(CURR))->type;
	}
	return res;
}

int assign()
{
	char *name;
	int tmp = 0;
	int res = 0;
	token_t *token = get_token(CURR);
	name = (char*)token->text;
	
	
	if (token->type == lcIDENT)
	{
		char *name;
		int tmp = 0;
		name = (char*)token->text;
		token_type type = (token = get_token(NEXT))->type;
		if (type == lcASSIGN || type == lcPLUS_ASSIGN ||
			type == lcMINUS_ASSIGN || type == lcMUL_ASSIGN ||
			type == lcDIV_ASSIGN)
		{
			get_token(NEXT);
			lookup(name, &tmp);
			switch (type)
			{
			case lcASSIGN:
				res = assign();
				break;
			case lcPLUS_ASSIGN:
				res = tmp += res = assign();
				break;
			case lcMINUS_ASSIGN:
				res = tmp -= assign();
				break;
			case lcMUL_ASSIGN:
				res = tmp *= assign();
				break;
			case lcDIV_ASSIGN:
				res = tmp /= assign();
				break;
			default:
				break;
			}
			assign_value(name, res);
			return res;
		}
		else
		{
			token = get_token(PREV);			
		}
	}
	res = relation();
	return res;

}