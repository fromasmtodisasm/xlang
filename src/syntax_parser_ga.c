#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#include "interpreter.h"
#include "interpreter_private.h"
#include "common.h"
#include "exp.h"
#include "lexer.h"
#include "preprocessor.h"
#ifdef __cplusplus
}
#endif // __cplusplus

#include <memory.h> //memcpy
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void skip_compound_statement(xlang_context* ctx);
void skip_statement(xlang_context* ctx);
int function_definition(xlang_context* ctx);
int declaration_list();
int start(xlang_context* ctx);

#define TAB_WIDTH 4
#define TYPES_CAPACITY 256
#define STACK_SIZE 64 
#define FUNCTIONS_COUNT 128 


//#define CREATE_PRIMITIVE_TYPE(n, t) string_ref_assign(tmp_type->name, n); tmp_type->object_type = PRIMITIVE; tmp_type->btype = t; add_type(global_context, tmp_type);

extern char *token_to_string[];
static type_t* tmp_type = NULL;
xlang_context* global_context;

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

token_type eat_tokens(xlang_context* ctx, token_type skip_to);

int gen_label() {
	static int label;
	return label++;
}

void exptected_func(char *exptected) {
  printf("Error. Expected %s\n", exptected);
}

void skip_if(xlang_context* ctx) {
  if (eat_tokens(ctx, lcRBRACE) == lcRBRACE) {
    lexer_get_token(ctx->lexer);
    skip_statement(ctx);
  }
}

void skip_while(xlang_context* ctx) {
  if (eat_tokens(ctx, lcRBRACE) == lcRBRACE) {
    lexer_get_token(ctx->lexer);
    skip_statement(ctx);
  }
}

token_type eat_tokens(xlang_context* ctx, token_type skip_to) {
  token_type type = lcEND;
  if ((type = CURRENT_TOKEN(ctx->lexer).type) == skip_to)
    return type;
  while (((type = lexer_get_token(ctx->lexer)->type) != skip_to) && type != lcEND)
    ;
  return type;
}
void skip_statement(xlang_context* ctx) {
  int stop = 0;
  while (!stop) {
    switch (CURRENT_TOKEN(ctx->lexer).type) {
    case lcLBRACKET: {
      skip_compound_statement(ctx);
      if (CURRENT_TOKEN(ctx->lexer).type != lcRBRACKET) {
        printf("error: expected }\n");
      }
      /*else
      {

      }*/
      return;
    } break;
    case lcIF: {
      skip_if(ctx);
    } break;
    case lcWHILE: {
      skip_while(ctx);
    } break;
    case lcRBRACKET:
      return;
    default:
      if (eat_tokens(ctx, lcSEMI) == lcSEMI)
        break;
      else
        return;
    }
    lexer_get_token(ctx->lexer);
  }
}

bool translation_unit(xlang_context* ctx)
{
  return start(ctx) != -1;
}

void skip_compound_statement(xlang_context* ctx) {
  int bracket_lvl = 0;
  int res = 0;
  token_t prev_token;

  prev_token = CURRENT_TOKEN(ctx->lexer);
  if (!(CURRENT_TOKEN(ctx->lexer).type == lcLBRACKET &&
        lexer_get_token(ctx->lexer)->type == lcRBRACKET) &&
      (CURRENT_TOKEN(ctx->lexer).type != lcEND)) {
    if (prev_token.type == lcLBRACKET) {
      skip_statement(ctx);
      if (CURRENT_TOKEN(ctx->lexer).type == lcRBRACKET) {

      } else {
        printf("error: expected }\n");
      }
    }
  }
}

way_out do_if(xlang_context* ctx) {
  int condition = 0;
  way_out out = NORMAL;

  match(ctx, lcLBRACE); 
  
  int endif = gen_label();
  int else_label = gen_label();
  // gencode
  assignment_expression();
  match(ctx, lcRBRACE);
  statement(ctx, SELECTION); 
  if (CURRENT_TOKEN(ctx->lexer).type == lcELSE) {
	match(ctx, lcELSE);
	statement(ctx, SELECTION);
	printf("_label_%04x:\n", else_label);
  }
  printf("_label_%04x:\n", endif);


 
  ///*lexer_get_token(ctx->lexer);*/
  return out;
}

way_out do_while(xlang_context* ctx) {
  int condition = 0;
  way_out out = NORMAL;

  match(ctx, lcLBRACE); 
  //assignment_expression();
  int header = gen_label();
  int end_while = gen_label();
    // gencode
  printf("_label_%04x:\n",header);
  //printf("\tcmp\n");
  assignment_expression();
  //printf("\tcmp\n");
  //printf("\tjnz _label_%04x\n", end_while);
  match(ctx, lcRBRACE);
  //lexer_get_token();

  statement(ctx, ITERATION);
  printf("\tjmp _label_%04x\n", header);
  printf("_label_%04x:\n",end_while);

  lexer_get_token(ctx->lexer);
  /*
	while (lexer_get_token(), condition = assignment_expression()) {
	  if ((CURRENT_TOKEN = CURRENT_TOKEN)->type == lcRBRACE) {
		lexer_get_token();
		if ((out = statement(ITERATION)) == BREAK) {
		  break;
		} else if (out == CONTINUE || out == NORMAL) {
		  pos_end = CURRENT_TOKEN(ctx->lexer).pos;
		  lexer_set_pos(pos_begin);
		  lexer_get_token();
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

void print_var_recursive(type_t* type, string_ref name, int level)
{
  type_t* curr_type;

  {
    switch (type->object_type)
    {
    case PRIMITIVE:
    {

      printf(
        "%*.stype [%.*s], name [%.*s]\n", level, "\t",
        type->name.len, type->name.pos,
        name.len, name.pos
      );
    }
    break;
    case STRUCT:
    {
      if (true)//!type->is_tag)
      {
        printf("%*.sStruct %.*s have %d fields:\n", level, " ", type->name.len, type->name.pos, type->num_types);
        for (int i = 0; i < type->num_types; i++)
        {
          print_var_recursive(&type->childrens[i], type->names[i], level + TAB_WIDTH);
        }
      }
    }
    break;
    default:
      FATAL_ERROR("unknown type");
      break;
    }
  }
}

void print_var(type_t* type, string_ref name, int level)
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

int start(xlang_context* ctx) {
  int retval = 0;

  exp_parser_init();

  string_ref name = string_ref_create("myCFunction");
  function_t* func = find_cfunction(ctx, name);

  //if (is_cfunction(ctx, name))
  //  call_cfunction(ctx, func);

  
  while (lexer_get_token(ctx->lexer)->type != lcEND) {
    switch (CURRENT_TOKEN(ctx->lexer).type)
    {
    case lcSTRUCT:
    {
      //type_t *new_type = &ctx->types[ctx->num_types - 1];
      type_t *new_type = create_type(global_context->global_types);
      struct_declaration(ctx, new_type);
      add_type(ctx, new_type);
      if (CURRENT_TOKEN(ctx->lexer).type != lcSEMI)
      {
        FATAL_ERROR("semi not found");
      }
      ctx->num_types++;
      print_var(new_type, new_type->name, 0);
    }
    break;
    default:
    {
      if (CURRENT_TOKEN(ctx->lexer).type == lcIDENT)
      {
        type_t *type = find_type(ctx->global_types, CURRENT_TOKEN(ctx->lexer).text);
        if (type != NULL)
        {
          //token_t prev_token = CURRENT_TOKEN(ctx->lexer);
          lexer_context_t* prev_lexer = NULL;
          lexer_save_context(&prev_lexer, ctx->lexer);
          if (lexer_get_token(ctx->lexer)->type == lcIDENT)
          {
            if (lexer_get_token(ctx->lexer)->type != lcLBRACE)
            {
              //CURRENT_TOKEN(ctx->lexer) = prev_token;
              //lexer_set_pos(ctx->lexer, prev_token.pos);
              lexer_update_context(&ctx->lexer, prev_lexer);
              //lexer_get_token(ctx->lexer);
              variable var;
              declare_variable(ctx, ctx->global_types, &var);
              if (CURRENT_TOKEN(ctx->lexer).type != lcSEMI)
              {
                FATAL_ERROR("semi not found");
              }
              variable *tmp = NULL;
              variable *cur_var = NULL;
              tmp = ctx->symbol_table;
              for (cur_var = ctx->symbol_table; cur_var != NULL; cur_var = cur_var->next)
              {
                tmp = cur_var;
                if (cur_var->name.len == 0)
                  break;
                if (!strncmp(name.pos, cur_var->name.pos, name.len))
                {
                  FATAL_ERROR("Redifinition variable");
                  break;
                }
              }

              *tmp = *create_varialble(var.name.pos);
              tmp->name.len = var.name.len;
              tmp->type = var.type;
              switch (tmp->type->object_type)
              {
              case PRIMITIVE:
              {
                tmp->ival = 0;
              }
              case STRUCT:
              {
                tmp->object = malloc(tmp->type->size);
              }
              break;
              default:
                break;
              }
            }
            else
            {
              //CURRENT_TOKEN(ctx->lexer) = prev_token;
              //lexer_set_pos(ctx->lexer, prev_token.pos);
              lexer_update_context(&ctx->lexer, prev_lexer);
              lexer_get_token(ctx->lexer);

              retval = function_definition(ctx);
            }
          }
          else
          {
            FATAL_ERROR("Expected declaration or definition");
          }
        }
      }
    }
    break;
    }
  }
  
  variable *cur_var = NULL;
  for (cur_var = ctx->symbol_table; cur_var != NULL; cur_var = cur_var->next)
  {
    printf("Var name is %.*s\n", cur_var->name.len, cur_var->name.pos);
    print_var(cur_var->type, cur_var->type->name, 0);
  }



  return retval;
}

int is_print() { return 0; }

int print(xlang_context* ctx) {
  int stop = 0;

  lexer_get_token(ctx->lexer);
  do {
    char *number = "%d";
    char *string = "%s";
    char *curtype;
    int expr_val = 0;
    if (CURRENT_TOKEN(ctx->lexer).type == lcSTRING) {
      printf("%.*s", CURRENT_TOKEN(ctx->lexer).text.len, CURRENT_TOKEN(ctx->lexer).text.pos);
      lexer_get_token(ctx->lexer);
    } else {
      curtype = number;
      expr_val = assignment_expression();
      printf("%d", expr_val);
    }
  } while (CURRENT_TOKEN(ctx->lexer).type == lcSTRING ||
           CURRENT_TOKEN(ctx->lexer).type == lcIDENT);
  puts("");
}

int read(xlang_context* ctx) {
  int stop = 0;
  int tmp;

  while (lexer_get_token(ctx->lexer)->type != lcSEMI &&
         CURRENT_TOKEN(ctx->lexer).type != lcEND) {
    if (CURRENT_TOKEN(ctx->lexer).type == lcIDENT) {
      scanf("%d", &tmp);
      assign_value(CURRENT_TOKEN(ctx->lexer).text, tmp);
    }
  }
}

int interprete(xlang_context* ctx) {
#if 0
  if (lexer_get_token(ctx->lexer)->type == lcSTRING) {
    start((char **)&(CURRENT_TOKEN(ctx->lexer).text));
  }
  lexer_get_token(ctx->lexer);
#endif
  return 0;
}

way_out statement(xlang_context* ctx, compound_origin origin) {
  int res = 0;
  int expr_len;
  way_out out = NORMAL;
  int stop = 0;

  while (!stop) {
    switch (CURRENT_TOKEN(ctx->lexer).type) {
    case lcIF: {
	  lexer_get_token(ctx->lexer);
      if ((out = do_if(ctx)) == CONTINUE || out == BREAK)
        return out;
    } break;
    case lcWHILE: {
	  lexer_get_token(ctx->lexer);
      do_while(ctx);
    } break;
    case lcBREAK: {
      if (lexer_get_token(ctx->lexer)->type == lcSEMI) {
        lexer_get_token(ctx->lexer);
        skip_statement(ctx);
        return BREAK;
      } else {
        exptected_func("SEMI");
      }
    } break;
    case lcGOTO: {
		match(ctx, lcIDENT);
		//gencode
		printf("\tjmp %s\n", CURRENT_TOKEN(ctx->lexer).text);
		match(ctx, lcSEMI);
    } break;
    case lcCONTINUE: {
      if (lexer_get_token(ctx->lexer)->type == lcSEMI) {
        lexer_get_token(ctx->lexer);
        skip_statement(ctx);
        return CONTINUE;
      } else {
        exptected_func("SEMI");
      }
    } break;
    case lcLBRACKET: {
      out = compound_statement(ctx, origin);
      if (CURRENT_TOKEN(ctx->lexer).type != lcRBRACKET) {
        printf("error: expected }\n");
      }
      return out;
    } break;
    case lcFUNCTION: {
      if (lexer_get_token(ctx->lexer)->type == lcIDENT) {
        func_decl();
      } else {
        exptected_func("IDENT");
      }
    } break;
    case lcPRINT: {
      print(ctx);
      if (CURRENT_TOKEN(ctx->lexer).type == lcSEMI) {
        lexer_get_token(ctx->lexer);
      }
    } break;
    case lcREAD: {
      read(ctx);

    } break;
    case lcABORT: {
      lexer_get_token(ctx->lexer);
      puts("This is abort!");
      out = -1;
      goto abort;
    } break;
    case lcINTERPRETE: {
      interprete(ctx);
    } break;
    case lcNUMBER: {
    case lcIDENT: 

      res = assignment_expression();
      if (CURRENT_TOKEN(ctx->lexer).type != lcSEMI) {
        exptected_func("SEMI");
        goto abort;
      }
      lexer_get_token(ctx->lexer);
    } break;
    default:
      stop = 1;
      break;
    }
  }

abort:
  return out;
}
way_out compound_statement(xlang_context* ctx, compound_origin origin) {
  way_out out = NORMAL;
  int expr_len;
  token_t prev_token;
  int retval = 0;

  prev_token = CURRENT_TOKEN(ctx->lexer);
  if (CURRENT_TOKEN(ctx->lexer).type == lcLBRACKET &&
      lexer_get_token(ctx->lexer)->type == lcRBRACKET) {
    retval = 0;
  } else {
    if (prev_token.type == lcLBRACKET) {
      out = statement(ctx, origin);
      if (CURRENT_TOKEN(ctx->lexer).type == lcRBRACKET) {

      } else {
        printf("error: expected }\n");
      }
    }
  }
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

int function_definition(xlang_context* ctx) {
  way_out out;
  token_type type = CURRENT_TOKEN(ctx->lexer).type;
  if (is_type(global_context->global_types, CURRENT_TOKEN(ctx->lexer).text) && lexer_get_token(ctx->lexer)->type == lcIDENT) {
    lexer_get_token(ctx->lexer);
    declaration_list(ctx);
    out = compound_statement(ctx, COMPOUND);
  }

  return 0;
}

int declaration_list(xlang_context* ctx) {
  token_type type;
  int retval = -1;
  if (CURRENT_TOKEN(ctx->lexer).type == lcLBRACE) {
    if (lexer_get_token(ctx->lexer)->type != lcRBRACE)
    {
      if (is_type(global_context->global_types, CURRENT_TOKEN(ctx->lexer).text)) {
        if (lexer_get_token(ctx->lexer)->type == lcIDENT) {
          while (lexer_get_token(ctx->lexer)->type == lcCOMMA &&
                 is_type(global_context->global_types, lexer_get_token(ctx->lexer)->text) &&
                 lexer_get_token(ctx->lexer)->type == lcIDENT)
            ;
          if (CURRENT_TOKEN(ctx->lexer).type != lcRBRACE) {
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
  lexer_get_token(ctx->lexer);
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
