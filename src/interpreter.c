#include "interpreter.h"
#include "common.h"
#include "exp.h"
#include "lexer.h"
#include "preprocessor.h"

#include <memory.h> //memcpy
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define FATAL_ERROR(str) (fprintf(stderr, "Fatal error on line %d: %s\n", str), exit(-1))
//#define MATCH(tok) curr_token->type != tok ? FATAL_ERROR("error") : get_token(); 
static void MATCH(token_type tok) 
{
    curr_token->type != tok ? FATAL_ERROR("error") : (void)get_token();
}
#define TYPES_CAPACITY 256
#define STACK_SIZE 64 
#define FUNCTIONS_COUNT 128 

// static token_t *curr_token;// = curr_token;
void skip_compound_statement();
void skip_statement();
int function_definition();
int declaration_list();

static void CREATE_PRIMITIVE_TYPE(char* name, builtin_types t);

//#define CREATE_PRIMITIVE_TYPE(n, t) string_ref_assign(tmp_type->name, n); tmp_type->object_type = PRIMITIVE; tmp_type->btype = t; add_type(global_context, tmp_type);

typedef struct function_t function_t;
typedef struct block_t function_body_t;

typedef struct function_t
{
  string_ref name;
  variable* args;
  type_t return_type;
  function_body_t* body;

}function_t;

typedef struct interpreter_context
{
    type_t* types;
    int num_types;
    int types_capacity;

    function_t* functions[FUNCTIONS_COUNT];
    function_t* entry_point;

    int stack[STACK_SIZE];
}interpreter_context;

extern char *token_to_string[];
static type_t* tmp_type = NULL;
interpreter_context* global_context;

interpreter_context* create_interpreter_context()
{
    interpreter_context *ctx = malloc(sizeof(interpreter_context));
    global_context = ctx;
    if (ctx != NULL)
    {
        ctx->num_types = 0;
        ctx->types_capacity = TYPES_CAPACITY;
        ctx->types = malloc(sizeof(type_t) * TYPES_CAPACITY);
    }
    return ctx;
}

static void add_type(interpreter_context* ctx, type_t* t)
{
    ctx->types[ctx->num_types++] = *t;
}

static type_t* create_type()
{
    type_t* result = malloc(sizeof(type_t));

    if (result != NULL)
    {
        string_ref_assign(result->name, "unknown");
        result->names = NULL;
        result->object_type = UNKNOWN_OBJECT;
        result->types = NULL;
    }
}


static void interpreter_init()
{
    create_interpreter_context();
    tmp_type = create_type();
    CREATE_PRIMITIVE_TYPE("int", INT_TYPE);
    CREATE_PRIMITIVE_TYPE("char", CHAR_TYPE);
    CREATE_PRIMITIVE_TYPE("uint", UINT_TYPE);
    CREATE_PRIMITIVE_TYPE("uchar", UCHAR_TYPE);
    CREATE_PRIMITIVE_TYPE("float", FLOAT_TYPE);
//#undef CREATE_PRIMITIVE_TYPE 
}

token_type eat_tokens(token_type skip_to);

int gen_label() {
	static int label;
	return label++;
}

void exptected_func(char *exptected) {
  printf("Error. Expected %s\n", exptected);
}

void skip_if() {
  if (eat_tokens(lcRBRACE) == lcRBRACE) {
    get_token(/*NEXT_TOKEN*/);
    skip_statement();
  }
}

void skip_while() {
  if (eat_tokens(lcRBRACE) == lcRBRACE) {
    get_token(/*NEXT_TOKEN*/);
    skip_statement();
  }
}

token_type eat_tokens(token_type skip_to) {
  token_type type = lcEND;
  if ((type = curr_token->type) == skip_to)
    return type;
  while (((type = get_token(/*NEXT_TOKEN*/)->type) != skip_to) && type != lcEND)
    ;
  return type;
}
void skip_statement() {
  int stop = 0;
  while (!stop) {
    switch (curr_token->type) {
    case lcLBRACKET: {
      skip_compound_statement();
      if (curr_token->type != lcRBRACKET) {
        printf("error: expected }\n");
      }
      /*else
      {

      }*/
      return;
    } break;
    case lcIF: {
      skip_if();
    } break;
    case lcWHILE: {
      skip_while();
    } break;
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

void skip_compound_statement() {
  int bracket_lvl = 0;
  int res = 0;
  token_t prev_token;

  memcpy(&prev_token, curr_token, sizeof(token_t));
  if (!(curr_token->type == lcLBRACKET &&
        get_token(/*NEXT_TOKEN*/)->type == lcRBRACKET) &&
      (curr_token->type != lcEND)) {
    if (prev_token.type == lcLBRACKET) {
      skip_statement();
      if (curr_token->type == lcRBRACKET) {

      } else {
        printf("error: expected }\n");
      }
    }
  }
}

way_out do_if() {
  int condition = 0;
  way_out out = NORMAL;

  MATCH(lcLBRACE); 
  
  int endif = gen_label();
  int else_label = gen_label();
  // gencode
  assignment_expression();
  MATCH(lcRBRACE);
  statement(SELECTION); 
  if (curr_token->type == lcELSE) {
	MATCH(lcELSE);
	statement(SELECTION);
	printf("_label_%04x:\n", else_label);
  }
  printf("_label_%04x:\n", endif);


 
  ///*get_token(/*NEXT_TOKEN*/);*/
  return out;
}

way_out do_while() {
  int condition = 0;
  way_out out = NORMAL;

  MATCH(lcLBRACE); 
  //assignment_expression();
  int header = gen_label();
  int end_while = gen_label();
    // gencode
  printf("_label_%04x:\n",header);
  //printf("\tcmp\n");
  assignment_expression();
  //printf("\tcmp\n");
  //printf("\tjnz _label_%04x\n", end_while);
  MATCH(lcRBRACE);
  //get_token();

  statement(ITERATION);
  printf("\tjmp _label_%04x\n", header);
  printf("_label_%04x:\n",end_while);

  get_token();
  /*
	while (get_token(), condition = assignment_expression()) {
	  if ((curr_token = curr_token)->type == lcRBRACE) {
		get_token();
		if ((out = statement(ITERATION)) == BREAK) {
		  break;
		} else if (out == CONTINUE || out == NORMAL) {
		  pos_end = curr_token->pos;
		  set_pos(pos_begin);
		  get_token();
		  continue;
		} else if (out == RETURN) {
		  return RETURN;
		}
	  }
	}
  }
  */

  return out;
}

int func_decl() { return 0; }

int start(char **buffer) {
  int retval = 0;

  exp_parser_init();
  interpreter_init();

  if ((lexerInit(*buffer)) != 0) {
    while (get_token(/*NEXT_TOKEN*/)->type != lcEND) {
      retval = function_definition();
    }
  }

  return retval;
}

int is_print() { return 0; }

int print() {
  int stop = 0;

  get_token(/*NEXT_TOKEN*/);
  do {
    char *number = "%d";
    char *string = "%s";
    char *curtype;
    int expr_val = 0;
    if (curr_token->type == lcSTRING) {
      printf("%s", curr_token->text);
      get_token(/*NEXT_TOKEN*/);
    } else {
      curtype = number;
      expr_val = assignment_expression();
      printf("%d", expr_val);
    }
  } while ((curr_token = curr_token)->type == lcSTRING ||
           curr_token->type == lcIDENT);
  puts("");
}

int read() {
  int stop = 0;
  int tmp;

  while (get_token(/*NEXT_TOKEN*/)->type != lcSEMI &&
         curr_token->type != lcEND) {
    if (curr_token->type == lcIDENT) {
      scanf("%d", &tmp);
      assign_value(curr_token->text, tmp);
    }
  }
}

int interprete() {
  if (get_token(/*NEXT_TOKEN*/)->type == lcSTRING) {
    start((char **)&(curr_token->text));
  }
  get_token(/*NEXT_TOKEN*/);
  return 0;
}

way_out statement(compound_origin origin) {
  int res = 0;
  int expr_len;
  way_out out = NORMAL;
  int stop = 0;

  while (!stop) {
    switch (curr_token->type) {
    case lcIF: {
	  get_token(/*NEXT_TOKEN*/);
      if ((out = do_if()) == CONTINUE || out == BREAK)
        return out;
    } break;
    case lcWHILE: {
	  get_token();
      do_while();
    } break;
    case lcBREAK: {
      if (get_token(/*NEXT_TOKEN*/)->type == lcSEMI) {
        get_token(/*NEXT_TOKEN*/);
        skip_statement();
        return BREAK;
      } else {
        exptected_func("SEMI");
      }
    } break;
    case lcGOTO: {
		MATCH(lcIDENT);
		//gencode
		printf("\tjmp %s\n", curr_token->text);
		MATCH(lcSEMI);
    } break;
    case lcCONTINUE: {
      if (get_token(/*NEXT_TOKEN*/)->type == lcSEMI) {
        get_token(/*NEXT_TOKEN*/);
        skip_statement();
        return CONTINUE;
      } else {
        exptected_func("SEMI");
      }
    } break;
    case lcLBRACKET: {
      out = compound_statement(origin);
      if (curr_token->type != lcRBRACKET) {
        printf("error: expected }\n");
      }
      return out;
    } break;
    case lcFUNCTION: {
      if (get_token(/*NEXT_TOKEN*/)->type == lcIDENT) {
        func_decl();
      } else {
        exptected_func("IDENT");
      }
    } break;
    case lcPRINT: {
      print();
      if ((curr_token = curr_token)->type == lcSEMI) {
        get_token(/*NEXT_TOKEN*/);
      }
    } break;
    case lcREAD: {
      read();

    } break;
    case lcABORT: {
      get_token(/*NEXT_TOKEN*/);
      puts("This is abort!");
      out = -1;
      goto abort;
    } break;
    case lcINTERPRETE: {
      interprete();
    } break;
    case lcNUMBER: {
    case lcIDENT: 

      res = assignment_expression();
      if (curr_token->type != lcSEMI) {
        exptected_func("SEMI");
        goto abort;
      }
      get_token(/*NEXT_TOKEN*/);
    } break;
    default:
      stop = 1;
      break;
    }
  }

abort:
  return out;
}
way_out compound_statement(compound_origin origin) {
  way_out out = NORMAL;
  int expr_len;
  token_t prev_token;
  int retval = 0;

  prev_token = *curr_token;
  if (curr_token->type == lcLBRACKET &&
      get_token(/*NEXT_TOKEN*/)->type == lcRBRACKET) {
    retval = 0;
  } else {
    if (prev_token.type == lcLBRACKET) {
      out = statement(origin);
      if (curr_token->type == lcRBRACKET) {

      } else {
        printf("error: expected }\n");
      }
    }
  }
  return out;
}

static bool find_type(interpreter_context* ctx, string_ref lexem)
{
    bool result = false;
    for (int i = 0; i < ctx->num_types; i++)
    {
        if (!strncmp(ctx->types[i].name.pos, lexem.pos, ctx->types[i].name.len))
        {
            result = true;
            break;
        }
    }
    return result;
}

int is_type(string_ref lexem) {
#if 0
  int res = FALSE;
  switch (type) {
  case lcINT:
  case lcVOID:
  case lcCHAR:
    res = TRUE;
    break;
  }
#endif
  
  return find_type(global_context, lexem);
}

int function_definition() {
  way_out out;
  token_type type = curr_token->type;
  if (is_type(curr_token->text) && get_token(/*NEXT_TOKEN*/)->type == lcIDENT) {
    get_token(/*NEXT_TOKEN*/);
    declaration_list();
    out = compound_statement(COMPOUND);
  }

  return 0;
}

int declaration_list() {
  token_type type;
  int retval = -1;
  if (curr_token->type == lcLBRACE) {
    if (get_token(/*NEXT_TOKEN*/)->type != lcRBRACE)
    {
      if (is_type(curr_token->text)) {
        if (get_token(/*NEXT_TOKEN*/)->type == lcIDENT) {
          while (get_token(/*NEXT_TOKEN*/)->type == lcCOMMA &&
                 is_type(get_token(/*NEXT_TOKEN*/)->text) &&
                 get_token(/*NEXT_TOKEN*/)->type == lcIDENT)
            ;
          if (curr_token->type != lcRBRACE) {
            exptected_func("RBRACE");
          } 
          else {
            retval = 0;
          }
        }
      } 
    }
    else {
      retval = 0;
    }
  }
  get_token(/*NEXT_TOKEN*/);
}

static void CREATE_PRIMITIVE_TYPE(char* name, builtin_types t)
{
    tmp_type->name = string_ref_create(name);
    tmp_type->object_type = PRIMITIVE;
    tmp_type->btype = t;
    add_type(global_context, tmp_type);
}
