#include "exp.h"
#include "lexer.h"
#include "interpreter.h"
#include "interpreter_private.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


DEFINE_LEXER_CONTEXT();
variable *vars;
extern xlang_context* global_context;

typedef struct primary_value_t
{
  enum primary_type
  {
    LITERAL,
    VARIABLE
  }type;
  union
  {
    variable* var;
    union 
    {
      char* sval;
      char cval;
      unsigned char ucval;
      int ival;
      unsigned int uival;
      float fval;
    }/*literal_value*/;
  };

}primary_value_t;

int make_builtin_vars()
{
	//vars = malloc(sizeof(variable));
  vars = global_context->symbol_table;
#if 0
  variable False =
  {
    .name = string_ref_create("false"),
    .ival = 0
	};
	variable True = {
		.name = string_ref_create("true"),
		.ival = 1,
	};

	memcpy(vars, &True, sizeof(variable));
  vars->next = malloc(sizeof(variable));
	memcpy(vars->next, &False, sizeof(variable));
#endif
}

int exp_parser_init()
{
	make_builtin_vars();
}

int block(char **buffer)
{
	return 0;
}

variable* lookup(string_ref name)
{
	variable *cur_var;
  //variable* vars = global_context->symbol_table;
	int res = 0;
	for (cur_var = vars; cur_var != NULL; cur_var = cur_var->next)
	{
		if (cur_var->name.len > 0)
			if (!strncmp(name.pos, cur_var->name.pos, name.len))
			{
        return cur_var;
			}
	}
	return NULL;
}

int assign_value(string_ref name, int val)
{
	variable *cur_var;
	variable *tmp;
	for (cur_var = vars; cur_var != NULL; cur_var = cur_var->next)
	{
		tmp = cur_var;
		if (cur_var->name.pos == NULL)
		{
			cur_var->name = name;
			cur_var->ival = val;
			break;
		}
		else if (!strncmp(name.pos, cur_var->name.pos, name.len))
		{
			cur_var->name = name;
			cur_var->ival = val;
			break;
		}
	}
	if (!cur_var)
	{
		tmp->next = malloc(sizeof(variable));
		tmp = tmp->next;
		tmp->name = name;
		tmp->ival = val;
		tmp->next = NULL;

	}
}

int primary_expression(xlang_context* ctx, primary_value_t *pv)
{
	int res = 0;
	int sign = 0; // -1 - negative, 0 without sign
	if (CURRENT_TOKEN(ctx->lexer).type == lcPLUS || CURRENT_TOKEN(ctx->lexer).type == lcMINUS) {
		if (CURRENT_TOKEN(ctx->lexer).type == lcMINUS)
			sign = -1;
		get_token(ctx->lexer);
	}
	switch (CURRENT_TOKEN(ctx->lexer).type)
	{

	case lcNUMBER:
	{
		//res = (int)CURRENT_TOKEN(ctx->lexer).text;
		//GENCODE
		printf("\tpush %c%d\n", sign==-1 ? '-' : ' ', atoi(CURRENT_TOKEN(ctx->lexer).text.pos));
		break;
	}
	case lcIDENT:
	{
    token_t prev_token = CURRENT_TOKEN(ctx->lexer);

    if (get_token(ctx->lexer)->type == lcLBRACE)
    {
      if (get_token(ctx->lexer)->type != lcRBRACE)
      {
        printf("error, rbrace expected\n");
        exit(-1);
      }
      if (is_cfunction(global_context, prev_token.text))
      {
        function_t* func = find_cfunction(global_context, prev_token.text);
        call_cfunction(global_context, func);
      }
    }
    else if (CURRENT_TOKEN(ctx->lexer).type == lcPOINT)
    {
      //type_t* this_type = find_type(global_context->global_types, prev_token.text);
      variable* var = lookup(prev_token.text);
      type_t* this_type = var->type;
      if (this_type != NULL)
      {
        pv->type = VARIABLE;
        pv->var = var;
        if (this_type->object_type == STRUCT)
        {
          get_token(ctx->lexer);
          primary_expression(ctx, pv);
        }
        else
        {
          FATAL_ERROR("type on left is not structure");
        }
      }
    }
		else 
		{
#if 0
      *CURRENT_TOKEN = prev_token;
      variable* var = lookup(CURRENT_TOKEN(ctx->lexer).text);
      if (type != NULL)
      {
        if (var == NULL)
        {
          printf("Error, undefined variable <%.*s>\n", CURRENT_TOKEN(ctx->lexer).text.len, CURRENT_TOKEN(ctx->lexer).text.pos);
        }
        else
        {

        }
      }
      else
      {
        FATAL_ERROR("This is not type");
      }
#endif
			//exit(-1);
		}
			//GENCODE
			printf("\tpush %c%.*s\n", sign==-1 ? '-' : ' ', CURRENT_TOKEN(ctx->lexer).text.len, CURRENT_TOKEN(ctx->lexer).text.pos);
		break;
	}
	case lcLBRACE:
	{
		get_token(ctx->lexer);
		res = assignment_expression(ctx);
		if (CURRENT_TOKEN(ctx->lexer).type != lcRBRACE)
		{
			printf("Error, expected ')'\n");
			getchar();
			//exit(-1);
		}
		break;
	}
	default:
		printf("Error, expected primary on line %d!!!\n", get_line(ctx->lexer));
		getchar();
		exit(-1);
	}
	get_token(ctx->lexer);
	return res;
}

int multiplicative_expression(xlang_context* ctx)
{
	int res = 0;
	int stop = 0;

  //type_t *type = create_type(global_context->global_types);
  primary_value_t pv;
	res = primary_expression(ctx, &pv);
	while ( CURRENT_TOKEN(ctx->lexer).type == lcMUL || CURRENT_TOKEN(ctx->lexer).type == lcDIV)
	{
		switch (CURRENT_TOKEN(ctx->lexer).type)
		{
		case lcMUL:
		{
			get_token(ctx->lexer);
			res *= primary_expression(ctx, &pv);
			//gencode
			printf("\tmul\n");
			break;
		}
		case lcDIV:
		{
			get_token(ctx->lexer);
			primary_expression(ctx, &pv);
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

int additive_expression(xlang_context* ctx)
{
	int res = 0;
	int stop = 0;
	
	res = multiplicative_expression(ctx);
	while (CURRENT_TOKEN(ctx->lexer).type == lcPLUS || CURRENT_TOKEN(ctx->lexer).type == lcMINUS)
	{
		switch (CURRENT_TOKEN(ctx->lexer).type)
		{
		case lcPLUS:
		{
			get_token(ctx->lexer);
			res += multiplicative_expression(ctx);
			//gencode
			printf("\tadd\n");
			break;
		}
		case lcMINUS:
		{
			get_token(ctx->lexer);
			res -= multiplicative_expression(ctx);
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

int conditional_expression(xlang_context* ctx)
{
	int res = 0;
	token_type type = CURRENT_TOKEN(ctx->lexer).type;
	res = additive_expression(ctx);
	
	while (is_relop(type=CURRENT_TOKEN(ctx->lexer).type))
	{
		get_token(ctx->lexer);
		switch (type)
		{	

		case lcAND_OP:
			res &= additive_expression(ctx);
			//gencode
			printf("\tcmp\n");
			//gencode
			printf("\tand\n");
			break;
		case lcOR_OP:
			res |= additive_expression(ctx);
			//gencode
			printf("\tcmp\n");
			//gencode
			printf("\tor\n");
			break;
		case lcEQ_OP:
			res = res == additive_expression(ctx);
			//gencode
			printf("\tcmp\n");
			//gencode
			printf("\tcmp\n");
			break;
		case lcL_OP:
			res  = res < additive_expression(ctx);
			//gencode
			printf("\tcmp\n");
			//gencode
			printf("\tjl\n");
			break;
		case lcG_OP:
			res = res > additive_expression(ctx);
			//gencode
			printf("\tcmp\n");
			//gencode
			printf("\tja\n");
			break;
		case lcLE_OP:
			res = res <= additive_expression(ctx);
			//gencode
			printf("\tcmp\n");
			//gencode
			printf("\tjbe\n");
			break;
		case lcGE_OP:
			res = res >= additive_expression(ctx);
			//gencode
			printf("\tcmp\n");
			//gencode
			printf("\tjae\n");
			break;
		case lcN_OP:
			res != additive_expression(ctx);
			//gencode
			printf("\tcmp\n");
			//gencode
			printf("\tjnz\n");
			break;
		case lcNE_OP:
			res = res != additive_expression(ctx);
			//gencode
			printf("\tcmp\n");
			//gencode
			printf("\tjne\n");
		default:
			break;
		}
		type = CURRENT_TOKEN(ctx->lexer).type;
	}
	return res;
}

int assignment_expression(xlang_context* ctx)
{
	string_ref name;
	int tmp = 0;
	int res = 0;
  lexer_context_t* lexer = NULL;

	name = CURRENT_TOKEN(ctx->lexer).text;
	if (CURRENT_TOKEN(ctx->lexer).type == lcSEMI)
		return res;
	if (CURRENT_TOKEN(ctx->lexer).type == lcIDENT)
	{
		int tmp = 0;
		string_ref name = CURRENT_TOKEN(ctx->lexer).text;
    lexer_save_context(&lexer, ctx->lexer);
		
		token_type type = get_token(ctx->lexer)->type;
		if (type == lcASSIGN || type == lcPLUS_ASSIGN ||
			type == lcMINUS_ASSIGN || type == lcMUL_ASSIGN ||
			type == lcDIV_ASSIGN)
		{
			get_token(ctx->lexer);
			lookup(name, &tmp);
			switch (type)
			{
			case lcASSIGN:
				res = assignment_expression(ctx);
				//gencode
				printf("\tpush %*.s\n", name.len, name);
				printf("\tsave\n");
				break;
			case lcPLUS_ASSIGN:
				res = tmp += res = assignment_expression(ctx);
				//gencode
				printf("\tpush %*.s\n", name.len, name);
				printf("\tload\n");
				printf("\tadd\n");
				printf("\tpush %*.s\n", name.len, name);
				printf("\tsave\n");
				
				break;
			case lcMINUS_ASSIGN:
				res = tmp -= assignment_expression(ctx);
				//gencode
				printf("\tpush %*.s\n", name.len, name);
				printf("\tload\n");
				printf("\tsub\n");
				printf("\tpush %*.s\n", name.len, name);
				printf("\tsave\n");
				break;
			case lcMUL_ASSIGN:
				res = tmp *= assignment_expression(ctx);
				//gencode
				printf("\tpush %*.s\n", name.len, name);
				printf("\tload\n");
				printf("\tmul\n");
				printf("\tpush %*.s\n", name.len, name);
				printf("\tsave\n");
				break;
			case lcDIV_ASSIGN:
				res = tmp /= assignment_expression(ctx);
				//gencode
				printf("\tpush %*.s\n", name.len, name);
				printf("\tload\n");
				printf("\tdiv\n");
				printf("\tpush %*.s\n", name.len, name);
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
      lexer_update_context(&ctx->lexer, lexer);
		}
	}
	res = conditional_expression(ctx);
	return res;

}
