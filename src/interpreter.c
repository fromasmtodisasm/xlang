#include "interpreter.h"
#include "common.h"
#include "preprocessor.h"
#include "generic_list.h"

#include <memory.h> //memcpy
#include <stdio.h>
#include <unistd.h> //sleep
#include <stdlib.h> //system

#define exptected_func(...) printf("On line %d for source line = %d\n", __LINE__, get_line());_expected_func( __VA_ARGS__)

extern char *token_to_string[];
int function_definition();
int declaration_list();
void print_statements(void *stmnt)
{
  node_t* st = (node_t*)stmnt;
  assert(st != NULL);
  printf("Current stmnt = %s\n", st->text);

}

static int is_type(token_type type);

token_type eat_tokens(token_type skip_to);

void _expected_func(char *exptected) {
  ERROR("Error. Expected %s\n", exptected);
}

way_out do_if(node_t **root) {
  node_t *if_node = NULL;
  node_t *statement_node = NULL;
  way_out out = NORMAL;

  if_node = *root;//create_node();
  if_node->text = curr_token->text; 
  if_node->type = curr_token->type;

  get_token(/*NEXT_TOKEN*/);
  if (curr_token->type == lcLBRACE) {
    get_token(/*NEXT_TOKEN*/);
    assignment_expression(&if_node->left);
    if (curr_token->type == lcRBRACE) {
      get_token(/*NEXT_TOKEN*/);
      statement_node = create_node();
      statement_node->type = lcLBRACKET;
      out = statement(&statement_node->left);
      if (get_token(/*NEXT_TOKEN*/)->type == lcELSE){
        get_token(/*NEXT_TOKEN*/);
        statement(&statement_node->right);
      }
    }
  }
  else { ERROR("Expected LBREACE\n"); }
  ///*get_token(/*NEXT_TOKEN*/);*/
  
  if_node->right = statement_node;
  *root = if_node;
  return out;
}

way_out do_while(node_t **root) {
  node_t *while_node = NULL;
  way_out out = NORMAL;

  while_node = *root;//create_node();
  while_node->text = curr_token->text; 
  while_node->type = curr_token->type;

  get_token(/*NEXT_TOKEN*/);
  if (curr_token->type == lcLBRACE) {
    get_token(/*NEXT_TOKEN*/);
    assignment_expression(&while_node->left);
    //get_token();
    if (curr_token->type == lcRBRACE) {
      get_token(/*NEXT_TOKEN*/);
      //printf("While get statement\n");
      statement(&while_node->right);
    }
  }
  else { ERROR("Expected RBRACE on line %d\n", get_line()); }

  *root = while_node;
  get_token(/*NEXT_TOKEN*/);
  return out;
}

int func_decl() { return 0; }
/*!
   \brief Это функция очень великая

*/
int start(char **buffer) {
  int retval = 0;

  exp_parser_init();
  struct list_t *functions;
  node_t *curr_func;

  if ((lexerInit(*buffer)) != 0) {
    while (get_token(/*NEXT_TOKEN*/)->type != lcEND) {
      retval = function_definition(&curr_func);
      printf("Curr_func name = %s\n", curr_func->text);
      assert(curr_func != NULL);
      push(&functions, curr_func, sizeof(node_t)); 
    }
  }

  printf("Func name = %s\n", curr_func->text);

  return retval;
}

int do_print(node_t **root) {
  int stop = 0;
  get_token(/*NEXT_TOKEN*/);
  do {
    char *number = "%f";
    char *string = "%s";
    char *curtype;
    node_t *expr_val = NULL;
    switch (curr_token->type) {
    case lcSTRING:
      printf("%s", curr_token->text);
      get_token(/*NEXT_TOKEN*/);
      break;

    case lcNUMBER:
    case lcIDENT:
      curtype = number;
      expr_val = eval();
      printf("%f", expr_val->value.f);
      break;
    default:
      puts("");
      return TRUE;
    }
  } while (TRUE);
}

int do_read() {
  int stop = 0;
  int tmp;

  while (get_token(/*NEXT_TOKEN*/)->type != lcSEMI &&
         curr_token->type != lcEND) {
    if (curr_token->type == lcIDENT) {
      scanf("%d", &tmp);
      //assign_value(curr_token->text, tmp);
    }
  }
}

void do_sleep() {
  int ms;
  get_token();
  ms = eval()->value.f;
  sleep(ms);
}

void do_pause(){
  puts("Pause, press any key");
  getchar();
}

int interprete() {
  if (get_token(/*NEXT_TOKEN*/)->type == lcSTRING) {
    start((char **)&(curr_token->text));
  }
  get_token(/*NEXT_TOKEN*/);
  return 0;
}

way_out statement(node_t **root) {
  int res = 0;
  int expr_len;
  way_out out = NORMAL;
  int stop = 0;
  struct node_t *statements;
  struct node_t *curr_statement;

  //printf("in function %s\n", __FUNCTION__);

  statements = curr_statement;
  while (TRUE) {
  //printf("curr_tok = %s\n", curr_token->text);
    curr_statement = create_node();
    curr_statement->text = strdup(curr_token->text);
    curr_statement->type = curr_token->type;
    switch (curr_token->type) {
    case lcIF: {
      do_if(&curr_statement);
    } break;
    case lcWHILE: {
      do_while(&curr_statement);
      printf("after while tok = %s\n", curr_statement->text);
    } break;
    case lcBREAK: {
      if (get_token(/*NEXT_TOKEN*/)->type == lcSEMI) {
        get_token(/*NEXT_TOKEN*/);
      } else {
        exptected_func("SEMI");
      }
    } break;
    case lcCONTINUE: {
      if (get_token(/*NEXT_TOKEN*/)->type == lcSEMI) {
        get_token(/*NEXT_TOKEN*/);
      } else {
        exptected_func("SEMI");
      }
    } break;
    case lcRETURN: {
      get_token();
      assignment_expression(&curr_statement->left);
      if (curr_token->type == lcSEMI) {
        get_token(/*NEXT_TOKEN*/);
      } else {
        exptected_func("SEMI");
      }
    } break;
    case lcLBRACKET: {
      out = compound_statement(&curr_statement);
      if (curr_token->type != lcRBRACKET) {
        ERROR("error: expected }\n");
      }
      (*root)->right = (void*)statements;
      for (curr_statement = statements; curr_statement != NULL; curr_statement = curr_statement->right)
      {
        assert(curr_statement->right != NULL);
        printf("Curr statement = %s\n", curr_statement->text);
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
      //printf("In print\n");
      do_print(root);
      if ((curr_token = curr_token)->type == lcSEMI) {
        get_token(/*NEXT_TOKEN*/);
      }
    } break;
    case lcREAD: {
      do_read();

    } break;
    case lcABORT: {
      get_token(/*NEXT_TOKEN*/);
      puts("This is abort!");
      out = -1;
      goto abort;
    }
    case lcSLEEP: {
      do_sleep();
      if (curr_token->type == lcSEMI) {
        get_token(/*NEXT_TOKEN*/);
      }
    } break;
    case lcPAUSE: {
      do_pause();
      if (curr_token->type == lcSEMI) {
        get_token(/*NEXT_TOKEN*/);
      }
    }
    break;
    case lcINTERPRETE: {
      interprete();
    } break;
    case lcIDENT:
    case lcNUMBER: {
      curr_statement = eval();
      if (curr_token->type != lcSEMI) {
        exptected_func("SEMI");
        goto abort;
      }
      // printf("exp evaluated\n");
      get_token(/*NEXT_TOKEN*/);
    } break;
    case lcVAR: {
      
      define_var(&curr_statement);
      if (curr_token->type == lcSEMI) {
        get_token();
      }
    } break;
    default: {
      //printf("Stmnt list: \n");
      //printList(statements, print_statements);
      //printf("after print\n");
      (*root)->right = statements;
      puts("here");
      for (curr_statement = statements; curr_statement != NULL; curr_statement = curr_statement->right)
      {
        //assert(curr_statement->right != NULL);
        printf("Curr statement = %s\n", curr_statement->text);
      }
      return NORMAL;
    }
    }
    //assert(curr_statement->text != NULL);
    //printf("Pushed stmnt %s\n", curr_statement->text);
    //push(&statements, curr_statement, sizeof(node_t));
    statements = curr_statement;
    curr_statement = statements->right;

  }
abort:
  return out;
}

way_out compound_statement(node_t **root) {
  way_out out = NORMAL;
  int expr_len;
  token_t prev_token;
  int retval = 0;
  //printf("in function %s\n", __FUNCTION__);
  memcpy(&prev_token, curr_token, sizeof(token_t));
  if (curr_token->type == lcLBRACKET &&
      get_token(/*NEXT_TOKEN*/)->type == lcRBRACKET) {
    retval = 0;
  } else {
    if (prev_token.type == lcLBRACKET) {
      out = statement(root);
      if (curr_token->type == lcRBRACKET) {

      } else {
        ERROR("error: expected }\n");
      }
    }
  }
  return out;
}

int is_type(token_type type) {
  int res = FALSE;
  switch (type) {
  case lcINT:
  case lcVOID:
  case lcCHAR:
    res = TRUE;
    break;
  }
  return res;
}

int function_definition(node_t **root) {
  way_out out;
  node_t *arg_list;
  node_t *block;
  node_t *function;
  function = create_node();
  token_type type = curr_token->type;
  function->type = type;
  if (is_type(type) && get_token(/*NEXT_TOKEN*/)->type == lcIDENT) {
    function->text = strdup(curr_token->text);
    printf("Curr func name = %s\n", function->text);
    get_token(/*NEXT_TOKEN*/);
    declaration_list(&arg_list);
    out = compound_statement(&block);
  }
  
  function->left = arg_list;
  function->right = block;
  *root = function;
  printf("end function = %s\n", (*root)->text);
  return 0;
}

int declaration_list() {
  token_type type;
  int retval = -1;
  if (curr_token->type == lcLBRACE) {
    if (is_type(get_token(/*NEXT_TOKEN*/)->type)) {
      if (get_token(/*NEXT_TOKEN*/)->type == lcIDENT) {
        while (get_token(/*NEXT_TOKEN*/)->type == lcCOMMA &&
               is_type(get_token(/*NEXT_TOKEN*/)->type) &&
               get_token(/*NEXT_TOKEN*/)->type == lcIDENT)
          ;
        if (curr_token->type != lcRBRACE) {
          exptected_func("RBRACE");
        } else {
          retval = 0;
        }
      }
    } else {
      if (curr_token->type != lcRBRACE) {
        exptected_func("RBRACE");
      } else {
        retval = 0;
      }
    }
  }
  get_token(/*NEXT_TOKEN*/);
}

int define_var(){
  int res = 0;
  int is_get_tok = 1;
  token_type type;
  char *varname = "";
  node_t *value = NULL; 
  if (get_token()->type == lcIDENT)
  {
    varname = strdup(curr_token->text);
    node_t *value;
    if (get_token()->type == lcASSIGN)
    {
      if ((type = get_token()->type) == lcIDENT || type == lcNUMBER)
      {
        value = eval();
        //assert(value != NULL);
        value->text = varname;
        assign_value(value);
      } 
    }
    else { 
      value = create_node();
      value->text = varname;
      value->value.f = 0.0;
      assign_value(value);
      is_get_tok = FALSE; 
    }
    assert(value != NULL);
    DEBUG("Defined var <%s> with value = [%f]\n", value->text, value->value.f);
    res = TRUE;
  }
  else
  {
    ERROR("Expected identifier\n");
  }
  if (is_get_tok)
    get_token();
  return res;
}
