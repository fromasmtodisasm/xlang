/***************************************************************************/
/****************************** Includes ***********************************/
/***************************************************************************/
#include <memory.h> //memcpy
#include <stdio.h>
//#include <unistd.h> //sleep
#include <stdlib.h> //system
#include <string.h>

#include "syntax_parser.h"
#include "interpreter_private.h"
#include "common.h"
#include "preprocessor.h"
#include "generic_list.h"
#include "debug.h"

/***************************************************************************/
/***************** Defines used by this module only ************************/
/***************************************************************************/
#define exptected_func(...) DEBUG_TRACE("On line %d for source line = %d\n", __LINE__, lexer_get_line(ctx));_expected_func( __VA_ARGS__)
#define GET_TOKEN() (DEBUG_TRACE("Get from %s\n", __FUNCTION__), lexer_get_token(NULL))

/***************************************************************************/
/************************* Global Variables ********************************/
/***************************************************************************/
extern char *token_to_string[];
static type_t* tmp_type = NULL;
xlang_context* global_context;
/***************************************************************************/
/*********************** Function Prototypes *******************************/
/******  (should be static, if not they should be in '.h' file) ************/
/***************************************************************************/
static int function_definition(xlang_context* ctx, node_t **root);
static int declaration_list(xlang_context* ctx, node_t **root);
type_t* find_type(type_t* scope, string_ref lexem);
int is_type(type_t* scope, string_ref lexem);
static int do_print(xlang_context* ctx, node_t **root);
void var_definition(xlang_context* ctx, node_t **root);
// Parsing
bool translation_unit(xlang_context* ctx);
// Initialization
xlang_context* create_interpreter_context();
// Utils
void print_var(type_t* type, string_ref name, int level);
variable* create_varialble(const char* name);
// Initialization
static void CREATE_PRIMITIVE_TYPE(char* name, builtin_types t, int size);
void call_cfunction(xlang_context* ctx, function_t* func);
void push_integer(xlang_context* ctx, int value);
/***************************************************************************/
void match(xlang_context* ctx, token_type tok)
{
    CURRENT_TOKEN(ctx->lexer).type != tok ? FATAL_ERROR("error") : (void)lexer_get_token(ctx->lexer);
}

int myCFunction(xlang_context* ctx)
{
  printf("this is %s\n", __FUNCTION__);
  push_integer(ctx, 123);
  return 1;
}

xlang_context* xlang_create()
{
  xlang_context *ctx = create_interpreter_context();
  if (ctx != NULL)
  {
    tmp_type = create_type(NULL);

    CREATE_PRIMITIVE_TYPE("int", INT_TYPE, 4);
    CREATE_PRIMITIVE_TYPE("char", CHAR_TYPE, 1);
    CREATE_PRIMITIVE_TYPE("uint", UINT_TYPE, 4);
    CREATE_PRIMITIVE_TYPE("uchar", UCHAR_TYPE, 1);
    CREATE_PRIMITIVE_TYPE("float", FLOAT_TYPE, 4);

	CREATE_PRIMITIVE_TYPE("char", CHAR_TYPE, 4);
	CREATE_PRIMITIVE_TYPE("uchar", UCHAR_TYPE, 4);
	CREATE_PRIMITIVE_TYPE("bool", BOOL_TYPE, 4);
	CREATE_PRIMITIVE_TYPE("int", INT_TYPE, 4);
	CREATE_PRIMITIVE_TYPE("uint", UINT_TYPE, 4);
	CREATE_PRIMITIVE_TYPE("float", FLOAT_TYPE, 4);
	CREATE_PRIMITIVE_TYPE("double", DOUBLE_TYPE, 4);

	CREATE_PRIMITIVE_TYPE("vec2", VEC2_TYPE, 2 * 4);
	CREATE_PRIMITIVE_TYPE("vec3", VEC3_TYPE, 3 * 4);
	CREATE_PRIMITIVE_TYPE("vec4", VEC4_TYPE, 4 * 4);

	CREATE_PRIMITIVE_TYPE("dvec2", DVEC2_TYPE, 2 * 8);
	CREATE_PRIMITIVE_TYPE("dvec3", DVEC3_TYPE, 3 * 8);
	CREATE_PRIMITIVE_TYPE("dvec4", DVEC4_TYPE, 4 * 8);

	CREATE_PRIMITIVE_TYPE("mat2", MAT2_TYPE, 2 * 2 * 4);
	CREATE_PRIMITIVE_TYPE("mat3", MAT3_TYPE, 3 * 3 * 4);
	CREATE_PRIMITIVE_TYPE("mat4", MAT4_TYPE, 4 * 4 * 4);

	CREATE_PRIMITIVE_TYPE("dmat2", DMAT2_TYPE, 2 * 2 * 8);
	CREATE_PRIMITIVE_TYPE("dmat3", DMAT3_TYPE, 3 * 3 * 8);
	CREATE_PRIMITIVE_TYPE("dmat4", DMAT4_TYPE, 4 * 4 * 8);

    register_cfunction(ctx, myCFunction, "myCFunction");
  }
  return ctx;
}

void xlang_set_buffer(xlang_context* ctx, char* buffer)
{
  ctx->source = buffer;
  lexer_set_pos(ctx->lexer, buffer);
}

bool xlang_parse(xlang_context* ctx)
{
  return translation_unit(ctx);
}

bool register_cfunction(xlang_context *ctx, CFunction* function, const char* name)
{
  function_t* func = &ctx->functions[ctx->num_funcs++];
  
  func->name = string_ref_create(name);
  func->cfunc = function;
  func->type = C_FUNCTION;
}

bool is_cfunction(xlang_context* ctx, string_ref name)
{
  for (int i = 0; i < ctx->num_funcs; i++)
  {
    if (!strncmp(ctx->functions[i].name.pos, name.pos, ctx->functions[i].name.len) && ctx->functions[i].type == C_FUNCTION)
    {
      return true;
    }
  }
  return false;
}

xlang_context* create_interpreter_context()
{
    xlang_context *ctx = malloc(sizeof(xlang_context));
    global_context = ctx;
    if (ctx != NULL)
    {
      ctx->source = NULL;
      ctx->num_types = 0;
      ctx->types_capacity = TYPES_CAPACITY;
      //ctx->global_types = malloc(sizeof(type_t));
      ctx->global_types = create_type(NULL);
      ctx->global_types->object_type = GLOBAL;
      ctx->global_types->num_types = 0;
      ctx->global_types->childrens = malloc(sizeof(type_t) * TYPES_CAPACITY);
      ctx->num_funcs = 0;
      ctx->entry_point = NULL;
      ctx->current_function = NULL;

      ctx->lexer = lexer_create_context();
      
      ctx->symbol_table = create_varialble("");
      ctx->sp = 0;
    }
    return ctx;
}

static void add_type(xlang_context* ctx, type_t* t)
{
  //t->p
  ctx->global_types->childrens[ctx->global_types->num_types++] = *t;
  ctx->num_types++;
}

type_t* create_type(type_t* parent)
{
    type_t* result = malloc(sizeof(type_t));

    if (result != NULL)
    {
        string_ref_assign(&result->name, "unknown");
        result->names = NULL;
        result->size = 0;
        result->object_type = UNKNOWN_OBJECT;
        result->parent_scope = parent;
        result->childrens = NULL;
        result->num_types = 0;
    }
}

bool translation_unit(xlang_context* ctx)
{
  return parse(ctx, NULL);
}


void _expected_func(/*xlang_context* ctx, */char *exptected) {
  ERROR("Error. Expected %s\n", exptected);
}

way_out do_if(xlang_context* ctx, node_t **root) {
  node_t *if_node = NULL;
  node_t *statement_node = NULL;
  way_out out = NORMAL;

  /*
   * Create and fill IF node
  */
  if_node = create_node(CURRENT_TOKEN(ctx->lexer).type, string_ref_create("if"));
  
  DEBUG_LOG("PARSE IF STATEMENT\n");
  lexer_get_token(ctx);
  if (CURRENT_TOKEN(ctx->lexer).type == lcLBRACE) {
    lexer_get_token(ctx);
    if_node->left = create_node(lcEXP, string_ref_create("expression"));
    if_node->left->right = eval(ctx);
    if (CURRENT_TOKEN(ctx->lexer).type == lcRBRACE) {
      lexer_get_token(ctx);
      statement_node = create_node(lcBLOCK, string_ref_create("block"));
      DEBUG_LOG("STATEMENT BLOCK CREATED\n");
      statement_node->left = create_node(lcSTMNT, string_ref_create("statement"));
      assert(statement_node->left != NULL);
      out = statement(ctx, &statement_node->left);
      DEBUG_TRACE("BEFORE ELSE\n");
      if (CURRENT_TOKEN(ctx->lexer).type == lcELSE){
        DEBUG_TRACE("AFTER ELSE");
        lexer_get_token(ctx);
        if_node->text = string_ref_create("ifelse");
        if_node->type = lcIFELSE;
        DEBUG_TRACE("CALL STATEMENT\n");
        statement_node->right = create_node(lcSTMNT, string_ref_create("statement"));
        statement(ctx, &statement_node->right);
        GET_TOKEN();
      }
      else { DEBUG_TRACE("WITHOUT ELSE\n"); }
    }
  }
  else { ERROR("Expected LBREACE\n"); }
  
  if_node->right = statement_node;
  *root = if_node;
  return out;
}

way_out do_while(xlang_context* ctx, node_t **root) {
  node_t *while_node = NULL;
  way_out out = NORMAL;

  while_node = create_node(CURRENT_TOKEN(ctx->lexer).type, string_ref_create("while"));
  puts("created while");
  lexer_get_token(ctx);
  if (CURRENT_TOKEN(ctx->lexer).type == lcLBRACE) {
    lexer_get_token(ctx);
    //assignment_expression(&while_node->left);
    while_node->left = create_node(lcEXP, string_ref_create("expression"));
    while_node->left->right = eval(ctx);
    //statement(&while_node->left);
    if (CURRENT_TOKEN(ctx->lexer).type == lcRBRACE) {
      lexer_get_token(ctx);
      statement(ctx, &while_node->right);
      assert(while_node->right != NULL);
      printf("text is %s\n",while_node->right->text);
      //puts("create while block");
    }
  }
  else { ERROR("Expected RBRACE on line %d\n", lexer_get_line(ctx)); }

  *root = while_node;
  lexer_get_token(ctx);
  return out;
}

static void print_var(type_t* type, string_ref name, int level)
{
  print_var_recursive(type, name, level);
  printf("Total size: %d\n", type->size);
  printf("***********\n");
}

variable* create_varialble(const char* name)
{
  variable* var = malloc(sizeof(variable));
  if (var != NULL)
  {
    var->name = string_ref_create(name);
    var->next = NULL;
    var->type = NULL;
  }
}

bool declare_variable(xlang_context* ctx, type_t *scope, variable* var)
{
  var->type = find_type(scope, CURRENT_TOKEN(ctx->lexer).text);
  if (lexer_get_token(ctx->lexer)->type == lcIDENT)
  {
    var->name = CURRENT_TOKEN(ctx->lexer).text;
    lexer_get_token(ctx->lexer);
    return true;
  }
  return false;
}

void struct_declaration(xlang_context* ctx, type_t *new_type)
{

  if (lexer_get_token(ctx->lexer)->type == lcIDENT)
  {
    new_type->object_type = STRUCT;
    new_type->name = CURRENT_TOKEN(ctx->lexer).text;
    if (lexer_get_token(ctx->lexer)->type == lcLBRACKET)
    {
      do
      {
        lexer_get_token(ctx->lexer);
        if (CURRENT_TOKEN(ctx->lexer).type == lcRBRACKET)
        {
          ;// print_var(new_type, new_type->name);
        }
        else if (CURRENT_TOKEN(ctx->lexer).type == lcSTRUCT)
        {
          type_t* nested_struct = create_type(new_type);
          nested_struct->is_tag = true;
          struct_declaration(ctx, nested_struct);

          new_type->num_types++;
          new_type->childrens = realloc(new_type->childrens, sizeof(type_t) * new_type->num_types);
          new_type->childrens[new_type->num_types - 1] = *nested_struct;
          if (CURRENT_TOKEN(ctx->lexer).type != lcSEMI)
          {
            if (lexer_get_token(ctx->lexer)->type == lcIDENT)
            {
              new_type->names = realloc(new_type->names, sizeof(string_ref) * new_type->num_types);
              new_type->names[new_type->num_types - 1] = nested_struct->name;
            }
            lexer_get_token(ctx->lexer);
          }
          else
          {

          }
        }
        else if (is_type(new_type, CURRENT_TOKEN(ctx->lexer).text))
        {
          variable var;
          if (declare_variable(ctx, new_type, &var))
          {
            new_type->num_types++;
            new_type->size += var.type->size;
            new_type->names = realloc(new_type->names, sizeof(string_ref) * new_type->num_types);
            new_type->names[new_type->num_types - 1] = var.name;
            new_type->childrens = realloc(new_type->childrens, sizeof(type_t) * new_type->num_types);
            new_type->childrens[new_type->num_types - 1] = *var.type;
          }
          else
          {
            FATAL_ERROR("could not declare struct field");
          }

        }
        else
        {
          FATAL_ERROR("this is not type");
        }
      } while (CURRENT_TOKEN(ctx->lexer).type == lcSEMI);
      lexer_get_token(ctx->lexer);
    }
  }
}

static void CREATE_PRIMITIVE_TYPE(char* name, builtin_types t, int size)
{
	tmp_type->name = string_ref_create(name);
	tmp_type->object_type = PRIMITIVE;
	tmp_type->btype = t;
	tmp_type->size = size;
	tmp_type->is_tag = false;
	add_type(global_context, tmp_type);
}

void call_cfunction(xlang_context* ctx, function_t* func)
{
	if (func != NULL)
	{
		int result = func->cfunc(ctx);
		printf("function results count = %d, value = %d\n", result, ctx->stack[ctx->sp - 1]);
		ctx->sp -= result;
	}
}

void push_integer(xlang_context* ctx, int value)
{
	if (ctx->sp < STACK_SIZE)
	{
		ctx->stack[ctx->sp++] = value;
	}
}

void pop_integer(xlang_context* ctx)
{
	if (ctx->sp >= 0)
	{
		ctx->sp++;
	}
}

function_t* find_cfunction(xlang_context* ctx, string_ref name)
{
	for (int i = 0; i < ctx->num_funcs; i++)
	{
		function_t* func = &ctx->functions[i];
		if (!strncmp(func->name.pos, name.pos, func->name.len))
		{
			return func;
		}
	}
}

node_t *parse(xlang_context *ctx) {
  node_t *external_defs = NULL;
  node_t *curr_node = NULL;
  node_t *program = NULL;
  token_type type;
  string_ref ident_name;
  DEBUG_TRACE("in function %s\n", __FUNCTION__); 
  external_defs = program = create_node(lcUNIT, string_ref_create("program")); 
  //printf("code type = %d\n", lcUNIT);
  
  exp_parser_init();
  
	while ((lexer_get_token(ctx)), CURRENT_TOKEN(ctx->lexer).type != lcEND) {
		DEBUG_TRACE("PARSE EXTERNAL DEFINITION\n");
		/**********************************************/ 
		type_t *type = find_type(ctx->global_types, CURRENT_TOKEN(ctx->lexer).text);
		if (type != NULL) {
			ident_name  = CURRENT_TOKEN(ctx->lexer).text;
			DEBUG_TRACE("Curr ident name = (%s)\n\n", ident_name);
			if (lexer_get_token(ctx)->type == lcLBRACE) {
				curr_node = create_node(lcFUNCTION, ident_name);
				function_definition(ctx, &curr_node);
			}
			else {
				curr_node = create_node(lcVARDEF, string_ref_create("var_def"));
				var_definition(ctx, &curr_node);
			}

		}
		else {
			ERROR("Expected function or var definition\n");
		}
		/**********************************************/ 
		if (curr_node != NULL) {
			DEBUG_TRACE("code of ident type = %d\n", curr_node->type);

			external_defs->right = curr_node;
			external_defs->left = create_node(curr_node->type, string_ref_create("external_def"));
			external_defs  = external_defs->left;
		}
	}

  if (program->right != NULL) {
    DEBUG_ALL("end. code of pr right = %d\n", program->right->type);
  }
  else { DEBUG_TRACE("EMPTY COMPILATION UNIT"); }
  return program;
}

int do_print(xlang_context* ctx, node_t **root) {
  int stop = 0;
  node_t *print_node = *root = create_node(lcPRINT, string_ref_create("print"));
  node_t *expr_val = NULL;
  print_node->text = CURRENT_TOKEN(ctx->lexer).text;
  DEBUG_TRACE("get next token\n");
  lexer_get_token(ctx);
  
  print_node->right = expr_val = create_node(lcEXP, string_ref_create("expression"));

  do {
    char *number = "%f";
    char *string = "%s";
    char *curtype;
    //node_t *expr_val = NULL;
    switch (CURRENT_TOKEN(ctx->lexer).type) {
    case lcSTRING:
      //puts("print");
      //printf("%s", CURRENT_TOKEN(ctx->lexer).text);
      lexer_get_token(ctx);
      break;

    case lcNUMBER:
    case lcIDENT:
      curtype = number;
      expr_val->right = eval(ctx);
      expr_val->left = create_node(lcEXP, string_ref_create("expression"));

      DEBUG_ALL("%f\n", expr_val->right->value.f);
      expr_val = expr_val->left;
      //puts("print");
      //assert(expr_val != NULL);
      //printf("%f", expr_val->value.f);
      break;
    default:
      puts("");
      return TRUE;
    }
  } while (TRUE);
  DEBUG_TRACE("LEAVE FUNCTION\n");
}

int do_read(xlang_context* ctx) {
  int stop = 0;
  int tmp;

  while (lexer_get_token(ctx)->type != lcSEMI &&
         CURRENT_TOKEN(ctx->lexer).type != lcEND) {
    if (CURRENT_TOKEN(ctx->lexer).type == lcIDENT) {
      scanf("%d", &tmp);
      //assign_value(CURRENT_TOKEN(ctx->lexer).text, tmp);
    }
  }
  return 0;
}

void do_sleep(xlang_context* ctx) {
  int ms;
  GET_TOKEN();
  ms = eval(ctx)->value.f;
  //sleep(ms);
}

void do_pause(xlang_context* ctx){
  puts("Pause, press any key");
  getchar();
}

//int do_interprete(xlang_context* ctx) {
//  if (lexer_get_token(ctx)->type == lcSTRING) {
//    parse((char **)&(CURRENT_TOKEN(ctx->lexer).text));
//  }
//  lexer_get_token(ctx);
//  return 0;
//}

void do_break(xlang_context* ctx, node_t **root) {
  node_t *break_node = *root = create_node(CURRENT_TOKEN(ctx->lexer).type, CURRENT_TOKEN(ctx->lexer).text);
  if (lexer_get_token(ctx)->type == lcSEMI) {
    lexer_get_token(ctx);
  } else {
    exptected_func("SEMI");
  } 
}

void do_continue(xlang_context* ctx, node_t **root) {
  node_t *continue_node = *root = create_node(CURRENT_TOKEN(ctx->lexer).type, CURRENT_TOKEN(ctx->lexer).text);
  if (lexer_get_token(ctx)->type == lcSEMI) {
    lexer_get_token(ctx);
  } else {
    exptected_func("SEMI");
  } 
}

void do_abort(xlang_context* ctx, node_t **root) {
  node_t *abort_node = *root = create_node(CURRENT_TOKEN(ctx->lexer).type, CURRENT_TOKEN(ctx->lexer).text);
  if (lexer_get_token(ctx)->type == lcSEMI) {
    lexer_get_token(ctx);
  } else {
    exptected_func("SEMI");
  } 
}

void do_return(xlang_context* ctx, node_t **root) {
  node_t *return_node = *root = create_node(CURRENT_TOKEN(ctx->lexer).type, CURRENT_TOKEN(ctx->lexer).text);
  GET_TOKEN();
  assignment_expression(ctx, &return_node->left);
  if (CURRENT_TOKEN(ctx->lexer).type == lcSEMI) {
    lexer_get_token(ctx);
  } else {
    exptected_func("SEMI");
  }

}

way_out statement(xlang_context* ctx, node_t **root) {
  int end_block = FALSE;      /* End of current block of statements */
  way_out out = NORMAL;
  node_t *statements = *root; /* List of statements */ 
  node_t *curr_statement;     /* Current recognized statement */
  DEBUG_TRACE("IN STATEMENT\n");
  //assert(*root != NULL);
  while (!end_block) {
    switch (CURRENT_TOKEN(ctx->lexer).type) {
    case lcIF: {
      do_if(ctx, &curr_statement);
    } break;
    case lcWHILE: {
      do_while(ctx, &curr_statement);
    } break;
    case lcBREAK: {
      do_break(ctx, &curr_statement);
    } break;
    case lcCONTINUE: {
      do_continue(ctx, &curr_statement); 
    } break;
    case lcRETURN: {
      do_return(ctx, &curr_statement); 
    } break;
    case lcLBRACKET: {
      out = compound_statement(ctx, &statements);
      *root = statements;
      DEBUG_TRACE("after comp tok = %s\n", statements->text);
      //curr_statement = statements;
      if (CURRENT_TOKEN(ctx->lexer).type != lcRBRACKET) {
        ERROR("error: expected }\n");
      }
      end_block = TRUE;
    } break;
    case lcPRINT: {
      DEBUG_TRACE("Parse print");
      do_print(ctx, &curr_statement);
      if (CURRENT_TOKEN(ctx->lexer).type == lcSEMI) {
        lexer_get_token(ctx);
      }
      DEBUG_TRACE("AFTER GET\n");
    } break;
    case lcREAD: {
      do_read(ctx);

    } break;
    case lcABORT: {
      do_abort(ctx, &curr_statement);
    } break;
    case lcSLEEP: {
      do_sleep(ctx);
      if (CURRENT_TOKEN(ctx->lexer).type == lcSEMI) {
        lexer_get_token(ctx);
      }
    } break;
    case lcPAUSE: {
      do_pause(ctx);
      if (CURRENT_TOKEN(ctx->lexer).type == lcSEMI) {
        lexer_get_token(ctx);
      }
    } break;
    /*case lcINTERPRETE: {
      do_interprete(ctx);
    } break;*/
    case lcIDENT: {
    case lcNUMBER: 
      curr_statement = create_node(lcEXP, string_ref_create("expression"));
      DEBUG_ALL("created expression");
      curr_statement->right = eval(ctx);
      if (CURRENT_TOKEN(ctx->lexer).type != lcSEMI) {
        exptected_func("SEMI");
      }
      lexer_get_token(ctx);
    } break;
    case lcVAR: {
      
      define_var(ctx, &curr_statement);
      if (CURRENT_TOKEN(ctx->lexer).type == lcSEMI) {
        GET_TOKEN();
      }
    } break;
    default: {

       
      out = NORMAL;
      end_block = TRUE;
      DEBUG_TRACE("DEFAULT MARK\n");
      //system("sleep 2");
      return out;
    } break;
    }
    assert(curr_statement != NULL);
    assert(statements != NULL);
    if (!end_block) {
      DEBUG_TRACE("THIS IS NOT END BLOCK\n");
      statements->right = curr_statement;
      statements->left = create_node(lcSTMNT, string_ref_create("statement"));
      statements  = statements->left;
    }

  }
  return out;
}

way_out compound_statement(xlang_context* ctx, node_t **root) {
  way_out out = NORMAL;
  token_t prev_token;
  node_t *block;
  DEBUG_TRACE("in function %s\n", __FUNCTION__);
  prev_token = CURRENT_TOKEN(ctx->lexer);
  if (CURRENT_TOKEN(ctx->lexer).type == lcLBRACKET) {
    block = create_node(lcBLOCK, string_ref_create("block"));
    //printf("created block\n");
    /**************************************/
    GET_TOKEN();
    block->right=create_node(lcSTMNT, string_ref_create("statement"));
    out = statement(ctx, &block->right);
    //printf("block right = %s\n", block->right->right->text);
    if (CURRENT_TOKEN(ctx->lexer).type == lcRBRACKET) {
      //puts("End compound!");
      //GET_TOKEN();
    } 
    else {
      ERROR("error: expected }\n");
    }
    /**************************************/
  } else {
    
  }
  //if (block->right != NULL) printf("block text = %s\n", block->right->text);
  //else printf("Empty block\n");
  DEBUG_TRACE("leave function %s\n", __FUNCTION__);
  *root = block;
  return out;
}

static type_t* find_type_recursive(type_t *type, string_ref lexem)
{
  if (type != NULL)
  {
    for (int i = 0; i < type->num_types; i++)
    {
      if (type->object_type == GLOBAL)
      {
        if (!strncmp(type->childrens[i].name.pos, lexem.pos, type->childrens[i].name.len))
        {
            return &type->childrens[i];
        }
      }
      else if (type->object_type == STRUCT)
      {
        if (type->childrens[i].object_type == STRUCT && !strncmp(type->childrens[i].name.pos, lexem.pos, type->childrens[i].name.len))
        {
            return &type->childrens[i];
        }
      }
    }
    return find_type_recursive(type->parent_scope, lexem);
  }
  return NULL;
}

type_t* find_type(type_t* scope, string_ref lexem)
{
  return find_type_recursive(scope, lexem);
}

int is_type(type_t* scope, string_ref lexem) {
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
  
  return find_type(scope, lexem) != NULL;
}

int function_definition(xlang_context* ctx, node_t **root) {
  way_out out;
  node_t *arg_list;
  node_t *block;
  node_t *function;
  node_t *tmp;
  function = *root;
  

  DEBUG_TRACE("Curr func name = (%s)\n\n", function->text);
  declaration_list(ctx, &arg_list);
  out = compound_statement(ctx, &block);

  assert(block != NULL);

  function->left = arg_list;
  function->right = block;
  DEBUG_TRACE("\nend function = (%s)", (*root)->text);
  return 0;
}

int declaration_list(xlang_context* ctx, node_t **root) {
  token_type type;
  int retval = -1;
  if (CURRENT_TOKEN(ctx->lexer).type == lcLBRACE) {
		lexer_get_token(ctx);
		type_t *type = find_type(ctx->global_types, CURRENT_TOKEN(ctx->lexer).text);
    if (type != NULL) {
      if (lexer_get_token(ctx)->type == lcIDENT) {
				while (true) {
					if (lexer_get_token(ctx)->type == lcCOMMA) {
						lexer_get_token(ctx);
						type_t *type = find_type(ctx->global_types, CURRENT_TOKEN(ctx->lexer).text);
						if (type != NULL) {
							if (lexer_get_token(ctx)->type == lcIDENT) {
								continue;
							}
						}
					}
					break;
				}
        if (CURRENT_TOKEN(ctx->lexer).type != lcRBRACE) {
          exptected_func("RBRACE");
        } else {
          retval = 0;
        }
      }
    } else {
      if (CURRENT_TOKEN(ctx->lexer).type != lcRBRACE) {
        exptected_func("RBRACE");
      } else {
        retval = 0;
      }
    }
  }
  lexer_get_token(ctx);
}

int define_var(xlang_context* ctx, node_t **root){
  int res = 0;
  int is_get_tok = 1;
  token_type type;
  string_ref varname;
  node_t *value = NULL; 
  if (GET_TOKEN()->type == lcIDENT)
  {
    node_t *value;
    varname = CURRENT_TOKEN(ctx->lexer).text;
    printf("varname = %s\n", varname);
    *root = create_node(lcVARDEF, string_ref_create("var_assign"));
    if (GET_TOKEN()->type == lcASSIGN)
    {
      if ((type = GET_TOKEN()->type) == lcIDENT || type == lcNUMBER || type == lcSTRING)
      {
        value = eval(ctx);
        assert(value != NULL);
        value->text = varname;
        assign_value(value);
      } 
    }
    else { 
      value = create_node(CURRENT_TOKEN(ctx->lexer).type, varname);
      value->value.f = 0.0;
      assign_value(value);
      is_get_tok = FALSE; 
    }
    assert(value != NULL);
    (*root)->left = value;

    DEBUG_ALL("Defined var <%s> with value = [%f]\n", value->text, value->value.f);
    res = TRUE;
  }
  else
  {
    ERROR("Expected identifier\n");
  }
  if (is_get_tok)
    GET_TOKEN();
  return res;
}

void var_definition(xlang_context* ctx, node_t **root) {
  //for( ;CURRENT_TOKEN(ctx->lexer).type != lcSEMI; GET_TOKEN());
  define_var(ctx, root);
}
