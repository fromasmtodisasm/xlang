#include "interpreter.h"
#include "lexer.h"
#include "exp.h"
#include "common.h"
#include "preprocessor.h"

#include <stdio.h>
#include <memory.h> //memcpy

extern char *token_to_string[];
//static token_t *curr_token;// = curr_token;
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
		get_token(/*NEXT_TOKEN*/);
		skip_statement();
	}
}

void skip_while()
{
	if (eat_tokens(lcRBRACE) == lcRBRACE)
	{
		get_token(/*NEXT_TOKEN*/);
		skip_statement();
	}
}

token_type eat_tokens(token_type skip_to)
{
	token_type type = lcEND;
	if ((type = curr_token->type) == skip_to)
		return type;
	while (((type = get_token(/*NEXT_TOKEN*/)->type) != skip_to) && type != lcEND);
	return type;
}
void skip_statement()
{
	int stop = 0;
	while (!stop)
	{
		switch (curr_token->type)
		{
		case lcLBRACKET:
		{
			skip_compound_statement();
			if (curr_token->type != lcRBRACKET)
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
		get_token(/*NEXT_TOKEN*/);
	}

}

void skip_compound_statement()
{
	int bracket_lvl = 0;
	int res = 0;
	token_t prev_token;

	memcpy(&prev_token, curr_token, sizeof(token_t));
	if (!(curr_token->type == lcLBRACKET && get_token(/*NEXT_TOKEN*/)->type == lcRBRACKET) && (curr_token->type != lcEND))
	{
		if (prev_token.type == lcLBRACKET)
		{
			skip_statement();
			if (curr_token->type == lcRBRACKET)
			{

			}
			else
			{
				printf("error: expected }\n");
			}
		}
	}
}

way_out do_if(node_t *root)
{
	int condition = 0;
	way_out out = NORMAL;
	
	get_token(/*NEXT_TOKEN*/);
	if (curr_token->type == lcLBRACE)
	{
		get_token(/*NEXT_TOKEN*/);
		condition = assignment_expression(root);
		if ((curr_token = curr_token)->type == lcRBRACE)
		{
			if (condition)
			{
				get_token(/*NEXT_TOKEN*/);
				if ((out = statement(SELECTION)) == CONTINUE || out == BREAK)
				{
					//get_token()/*FIXME: change process getting <PREV_TOKEN>*/;
				}
				else
				if (get_token(/*NEXT_TOKEN*/)->type == lcELSE)
				{
					get_token(/*NEXT_TOKEN*/);
					skip_statement();
				}
			}
			else
			{
				get_token(/*NEXT_TOKEN*/);
				skip_statement();
				if (get_token(/*NEXT_TOKEN*/)->type == lcELSE)
				{
					get_token(/*NEXT_TOKEN*/);
					out = statement(SELECTION);
				}
			}
		}
	}
	///*get_token(/*NEXT_TOKEN*/);*/
	return out;
}

way_out do_while(node_t *root)
{
	int condition = 0;
	way_out out = NORMAL;
	
	get_token(/*NEXT_TOKEN*/);
	if (curr_token->type == lcLBRACE)
	{
		char *pos_begin = curr_token->pos;
		char* pos_end = pos_begin;
		
		while (get_token(/*NEXT_TOKEN*/), condition = assignment_expression(root))
		{	
			if ((curr_token = curr_token)->type == lcRBRACE)
			{
				get_token(/*NEXT_TOKEN*/);
				if ((out = statement(ITERATION)) == BREAK)
				{
					break;
				}
				else if (out == CONTINUE || out == NORMAL)
				{
					pos_end = curr_token->pos;
					set_pos(pos_begin);
					get_token(/*NEXT_TOKEN*/);
					continue;
				}
				else if (out == RETURN)
				{
					return RETURN;
				}
			}
		}
		get_token(/*NEXT_TOKEN*/);
		skip_statement();
	}

	get_token(/*NEXT_TOKEN*/);
	return out;
}



int func_decl()
{
	return 0;
}

int start(char **buffer)
{
#define TEST
	int retval = 0;

	exp_parser_init();

	if ((lexerInit(*buffer)) != 0)
	{
#ifdef TEST
    node_t *root;
    printf("eval expression\n");
    while (get_token()->type != lcEND)
    {
      retval = assignment_expression(&root);
      //printf("exp val = %d\n", retval);
      //printf("%s\n", root->text);
      puts("Printing sytax tree...");
      
      prefix_tree(root,0);
      //printf("str = \n%s\n-----\n", get_pos());
    }
#else
		while (get_token(/*NEXT_TOKEN*/)->type != lcEND)
		{
			retval = function_definition();
		} 	
#endif
	}

	return retval;
}

int is_print()
{
	return 0;
}

int print(node_t *root)
{ 
	int stop = 0;
	
	get_token(/*NEXT_TOKEN*/);
	do
	{
		char *number = "%d";
		char *string = "%s";
		char *curtype;
		int expr_val = 0;
		if (curr_token->type == lcSTRING) {
			printf("%s", curr_token->text);
			get_token(/*NEXT_TOKEN*/);
		}
		else {
			curtype = number;
			expr_val = assignment_expression(&root);
			printf("%d", expr_val);
		}
	} while ((curr_token = curr_token)->type == lcSTRING || curr_token->type == lcIDENT);
	puts("");
}


int read()
{
	int stop = 0;
	int tmp;

	while (get_token(/*NEXT_TOKEN*/)->type != lcSEMI && curr_token->type != lcEND)
	{
		if (curr_token->type == lcIDENT)
		{
			scanf("%d", &tmp);
			assign_value(curr_token->text, tmp);

		}
	} 
}

int interprete()
{
	if (get_token(/*NEXT_TOKEN*/)->type == lcSTRING)
	{
		start((char**)&(curr_token->text));
	}
	get_token(/*NEXT_TOKEN*/);
	return 0;
}

way_out statement(compound_origin origin)
{
	int res = 0;
	int expr_len;
	way_out out = NORMAL;
	int stop = 0;

  node_t *root;

	while (!stop)
	{
		switch (curr_token->type)
		{
		case lcIF:
		{
			if ((out = do_if(root)) == CONTINUE || out == BREAK)
				return out;
		}
		break;
		case lcWHILE:
		{
			do_while(root);
		}
		break;
		case lcBREAK:
		{
			if (get_token(/*NEXT_TOKEN*/)->type == lcSEMI)
			{
				get_token(/*NEXT_TOKEN*/);
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
			if (get_token(/*NEXT_TOKEN*/)->type == lcSEMI)
			{
				get_token(/*NEXT_TOKEN*/);
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
			if (curr_token->type != lcRBRACKET)
			{
				printf("error: expected }\n");
			}
			return out;
		}
		break;
		case lcFUNCTION:
		{
			if (get_token(/*NEXT_TOKEN*/)->type == lcIDENT)
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
			print(root);
			if ((curr_token = curr_token)->type == lcSEMI)
			{
				get_token(/*NEXT_TOKEN*/);
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
			get_token(/*NEXT_TOKEN*/);
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

			res = assignment_expression(root);
			if (curr_token->type != lcSEMI)
			{
				exptected_func("SEMI");
				goto abort;
			}
			get_token(/*NEXT_TOKEN*/);
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
	token_t prev_token;
	int retval = 0;

	memcpy(&prev_token, curr_token, sizeof(token_t));
	if (curr_token->type == lcLBRACKET && get_token(/*NEXT_TOKEN*/)->type == lcRBRACKET)
	{
		retval = 0;
	}
	else
	{
		if (prev_token.type == lcLBRACKET)
		{
			out = statement(origin);
			if (curr_token->type == lcRBRACKET)
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
	token_type type = curr_token->type;
	if (is_type(type) && get_token(/*NEXT_TOKEN*/)->type == lcIDENT)
	{
		get_token(/*NEXT_TOKEN*/);
		declaration_list();
		out = compound_statement(COMPOUND);
	}

	return 0;
}

int declaration_list()
{
	token_type type;
	int retval = -1;
	if (curr_token->type == lcLBRACE)
	{
		if (is_type(get_token(/*NEXT_TOKEN*/)->type))
		{
			if (get_token(/*NEXT_TOKEN*/)->type == lcIDENT)
			{
				while (get_token(/*NEXT_TOKEN*/)->type == lcCOMMA && get_token(/*NEXT_TOKEN*/)->type == lcINT && get_token(/*NEXT_TOKEN*/)->type == lcIDENT);
				if (curr_token->type != lcRBRACE)
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
			if (curr_token->type != lcRBRACE)
			{
				exptected_func("RBRACE");
			}
			else
			{
				retval = 0;
			}

		}
	}
	get_token(/*NEXT_TOKEN*/);
}
