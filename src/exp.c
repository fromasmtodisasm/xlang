#include "exp.h"
#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


variable *vars;
//static token_t *curr_token;// = curr_token;

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

int exp_parser_init()
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

int primary_expression()
{
	int res = 0;
	int sign = 0; // -1 - negative, 0 without sign
	if (curr_token->type == lcPLUS || curr_token->type == lcMINUS) {
		if (curr_token->type == lcMINUS)
			sign = -1;
		get_token();
	}
	switch (curr_token->type)
	{

	case lcNUMBER:
	{
		//res = (int)curr_token->text;
		//GENCODE
		printf("\tpush %c%d\n", sign==-1 ? '-' : ' ', atoi(curr_token->text));
		break;
	}
	case lcIDENT:
	{
		if ((lookup(curr_token->text, &res)) == 0)
		{
			printf("Error, undefined variable <%s>\n", curr_token->text);

			//exit(-1);
		}
			//GENCODE
			printf("\tpush %c%s\n", sign==-1 ? '-' : ' ', curr_token->text);
		break;
	}
	case lcLBRACE:
	{
		get_token(/*NEXT_TOKEN*/);
		res = assignment_expression();
		if (curr_token->type != lcRBRACE)
		{
			printf("Error, expected ')'\n");
			getchar();
			//exit(-1);
		}
		break;
	}
	default:
		printf("Error, expected primary on line %d!!!\n", get_line());
		getchar();
		exit(-1);
	}
	get_token();
	return res;
}

int multiplicative_expression()
{
	int res = 0;
	int stop = 0;

	res = primary_expression();
	while ( curr_token->type == lcMUL || curr_token->type == lcDIV)
	{
		switch (curr_token->type)
		{
		case lcMUL:
		{
			get_token(/*NEXT_TOKEN*/);
			res *= primary_expression();
			//gencode
			printf("\tmul\n");
			break;
		}
		case lcDIV:
		{
			get_token(/*NEXT_TOKEN*/);
			primary_expression();
			//gencode
			printf("\tdiv\n");
			break;
		}
		default:
			stop = 1;
			break;
		}
	}
	return res;
}

int additive_expression()
{
	int res = 0;
	int stop = 0;
	
	res = multiplicative_expression();
	while (curr_token->type == lcPLUS || curr_token->type == lcMINUS)
	{
		switch (curr_token->type)
		{
		case lcPLUS:
		{
			get_token(/*NEXT_TOKEN*/);
			res += multiplicative_expression();
			//gencode
			printf("\tadd\n");
			break;
		}
		case lcMINUS:
		{
			get_token(/*NEXT_TOKEN*/);
			res -= multiplicative_expression();
			//gencode
			printf("\tsub\n");
			break;
		}
		default:
			stop = 1;
			break;
		}
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
			lcG_OP == type ||
			lcEQ_OP == type ||
			lcL_OP == type ||
			lcNE_OP == type
		);
}

int conditional_expression()
{
	int res = 0;
	token_type type = curr_token->type;
	res = additive_expression();
	
	while (is_relop(type=curr_token->type))
	{
		get_token(/*NEXT_TOKEN*/);
		switch (type)
		{	

		case lcAND_OP:
			res &= additive_expression();
			//gencode
			printf("\tcmp\n");
			//gencode
			printf("\tand\n");
			break;
		case lcOR_OP:
			res |= additive_expression();
			//gencode
			printf("\tcmp\n");
			//gencode
			printf("\tor\n");
			break;
		case lcEQ_OP:
			res = res == additive_expression();
			//gencode
			printf("\tcmp\n");
			//gencode
			printf("\tcmp\n");
			break;
		case lcL_OP:
			res  = res < additive_expression();
			//gencode
			printf("\tcmp\n");
			//gencode
			printf("\tjl\n");
			break;
		case lcG_OP:
			res = res > additive_expression();
			//gencode
			printf("\tcmp\n");
			//gencode
			printf("\tja\n");
			break;
		case lcLE_OP:
			res = res <= additive_expression();
			//gencode
			printf("\tcmp\n");
			//gencode
			printf("\tjbe\n");
			break;
		case lcGE_OP:
			res = res >= additive_expression();
			//gencode
			printf("\tcmp\n");
			//gencode
			printf("\tjae\n");
			break;
		case lcN_OP:
			res != additive_expression();
			//gencode
			printf("\tcmp\n");
			//gencode
			printf("\tjnz\n");
			break;
		case lcNE_OP:
			res = res != additive_expression();
			//gencode
			printf("\tcmp\n");
			//gencode
			printf("\tjne\n");
		default:
			break;
		}
		type = curr_token->type;
	}
	return res;
}

int assignment_expression()
{
	char *name;
	int tmp = 0;
	int res = 0;
	token_t prev_token;

	name = curr_token->text;
	if (curr_token->type == lcSEMI)
		return res;
	if (curr_token->type == lcIDENT)
	{
		char *name;
		int tmp = 0;
		name = curr_token->text;
		char *prev_pos = get_pos();
		memcpy(&prev_token, curr_token, sizeof(token_t));
		
		token_type type = get_token()->type;
		if (type == lcASSIGN || type == lcPLUS_ASSIGN ||
			type == lcMINUS_ASSIGN || type == lcMUL_ASSIGN ||
			type == lcDIV_ASSIGN)
		{
			get_token(/*NEXT_TOKEN*/);
			lookup(name, &tmp);
			switch (type)
			{
			case lcASSIGN:
				res = assignment_expression();
				//gencode
				printf("\tpush %s\n", name);
				printf("\tsave\n");
				break;
			case lcPLUS_ASSIGN:
				res = tmp += res = assignment_expression();
				//gencode
				printf("\tpush %s\n", name);
				printf("\tload\n");
				printf("\tadd\n");
				printf("\tpush %s\n", name);
				printf("\tsave\n");
				
				break;
			case lcMINUS_ASSIGN:
				res = tmp -= assignment_expression();
				//gencode
				printf("\tpush %s\n", name);
				printf("\tload\n");
				printf("\tsub\n");
				printf("\tpush %s\n", name);
				printf("\tsave\n");
				break;
			case lcMUL_ASSIGN:
				res = tmp *= assignment_expression();
				//gencode
				printf("\tpush %s\n", name);
				printf("\tload\n");
				printf("\tmul\n");
				printf("\tpush %s\n", name);
				printf("\tsave\n");
				break;
			case lcDIV_ASSIGN:
				res = tmp /= assignment_expression();
				//gencode
				printf("\tpush %s\n", name);
				printf("\tload\n");
				printf("\tdiv\n");
				printf("\tpush %s\n", name);
				printf("\tsave\n");
				break;
			default:
				break;
			}
			assign_value(name, res);
			return res;
		}
		else
		{
			memcpy(curr_token, &prev_token, sizeof(token_t));
			set_pos(prev_pos);
		}
	}
	res = conditional_expression();
	return res;

}
