/***************************************************************************/
/****************************** Includes ***********************************/
/***************************************************************************/
#include <memory.h> //memcpy
#include <stdio.h>
#include <unistd.h> //sleep
#include <stdlib.h> //system

#include "syntax_parser.h"
#include "common.h"
#include "preprocessor.h"
#include "generic_list.h"
#include "debug.h"

#define exptected_func(...) printf("On line %d for source line = %d\n", __LINE__, get_line());_expected_func( __VA_ARGS__)

/***************************************************************************/
/************************* Global Variables ********************************/
/***************************************************************************/
extern char *token_to_string[];

/***************************************************************************/
/*********************** Function Prototypes *******************************/
/******  (should be static, if not they should be in '.h' file) ************/
/***************************************************************************/
static int function_definition();
static int declaration_list();
static int is_type(token_type type);
static int do_print(node_t **root);
void var_definition(node_t **root);
static void print_statements(node_t *node, int leve);
/***************************************************************************/

void _expected_func(char *exptected) {
  ERROR("Error. Expected %s\n", exptected);
}

way_out do_if(node_t **root) {
  node_t *if_node = NULL;
  node_t *statement_node = NULL;
  way_out out = NORMAL;

  /*
   * Create and fill IF node
  */
  if_node = create_node(curr_token->type, "if"/*strdup(curr_token->text)*/);

  get_token(/*NEXT_TOKEN*/);
  if (curr_token->type == lcLBRACE) {
    get_token(/*NEXT_TOKEN*/);
    if_node->left = create_node(lcEXP, "expression");
    if_node->left->right = eval();
    if (curr_token->type == lcRBRACE) {
      get_token(/*NEXT_TOKEN*/);
      statement_node = create_node(lcBLOCK, "block");
      out = statement(&statement_node->left);
      if (get_token(/*NEXT_TOKEN*/)->type == lcELSE){
        get_token(/*NEXT_TOKEN*/);
        if_node->text = "IFELSE";
        if_node->type = lcIFELSE;
        statement(&statement_node->right);
        get_token();
      }
    }
  }
  else { ERROR("Expected LBREACE\n"); }
  
  if_node->right = statement_node;
  *root = if_node;
  return out;
}

way_out do_while(node_t **root) {
  node_t *while_node = NULL;
  way_out out = NORMAL;

  while_node = create_node(curr_token->type, "while");
  puts("created while");
  get_token(/*NEXT_TOKEN*/);
  if (curr_token->type == lcLBRACE) {
    get_token(/*NEXT_TOKEN*/);
    //assignment_expression(&while_node->left);
    while_node->left = create_node(lcEXP, "expression");
    while_node->left->right = eval();
    //statement(&while_node->left);
    if (curr_token->type == lcRBRACE) {
      get_token(/*NEXT_TOKEN*/);
      statement(&while_node->right);
      assert(while_node->right != NULL);
      printf("text is %s\n",while_node->right->text);
      //puts("create while block");
    }
  }
  else { ERROR("Expected RBRACE on line %d\n", get_line()); }

  *root = while_node;
  get_token(/*NEXT_TOKEN*/);
  return out;
}

node_t *parse(char **buffer) {
  node_t *external_defs = NULL;
  node_t *curr_node = NULL;
  node_t *program = NULL;
  token_type type;
  char *ident_name = NULL;
  DEBUG_TRACE("in function %s\n", __FUNCTION__); 
  external_defs = program = create_node(lcUNIT, "program"); 
  //printf("code type = %d\n", lcUNIT);
  
  exp_parser_init();
  if ((lexerInit(*buffer)) != 0) {
    while (get_token(/*NEXT_TOKEN*/)->type != lcEND) {
      /**********************************************/ 
      type = curr_token->type;
      if (is_type(type) && get_token(/*NEXT_TOKEN*/)->type == lcIDENT) {
        ident_name  = strdup(curr_token->text);
        DEBUG_TRACE("Curr ident name = (%s)\n\n", ident_name);
        if (get_token(/*NEXT_TOKEN*/)->type == lcLBRACE) {
          curr_node = create_node(lcFUNCTION, ident_name);
          function_definition(&curr_node);
        }
        else {
          curr_node = create_node(lcVARDEF, "var_def");
          var_definition(&curr_node);
        }

      }
      else {
        ERROR("Expected function or var definition\n");
      }
      /**********************************************/ 
      DEBUG_ALL("code of ident type = %d\n", curr_node->type);
      external_defs->right = curr_node;
      external_defs->left = create_node(curr_node->type, "external_def");
      external_defs  = external_defs->left;
    }
  }
  DEBUG_ALL("end. code of pr right = %d\n", program->right->type);
  return program;
}

int do_print(node_t **root) {
  int stop = 0;
  node_t *print_node = *root = create_node(lcPRINT, "print");
  print_node->text = strdup(curr_token->text);
  get_token(/*NEXT_TOKEN*/);

  /*
      curr_statement = create_node(lcEXP, "expression");
      puts("created expression");
      curr_statement->right = eval();
  node_t *expr_val = NULL;
  print_node->right = expr_val;
  */

  node_t *expr_val = NULL;
  print_node->right = expr_val = create_node(lcEXP, "expression");

  do {
    char *number = "%f";
    char *string = "%s";
    char *curtype;
    //node_t *expr_val = NULL;
    switch (curr_token->type) {
    case lcSTRING:
      //puts("print");
      //printf("%s", curr_token->text);
      get_token(/*NEXT_TOKEN*/);
      break;

    case lcNUMBER:
    case lcIDENT:
      curtype = number;
      expr_val->right = eval();
      expr_val->left = create_node(lcEXP, "expression");

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

int do_interprete() {
  if (get_token(/*NEXT_TOKEN*/)->type == lcSTRING) {
    parse((char **)&(curr_token->text));
  }
  get_token(/*NEXT_TOKEN*/);
  return 0;
}

void do_break(node_t **root) {
  node_t *break_node = *root = create_node(curr_token->type, strdup(curr_token->text));
  if (get_token(/*NEXT_TOKEN*/)->type == lcSEMI) {
    get_token(/*NEXT_TOKEN*/);
  } else {
    exptected_func("SEMI");
  } 
}

void do_continue(node_t **root) {
  node_t *continue_node = *root = create_node(curr_token->type, strdup(curr_token->text));
  if (get_token(/*NEXT_TOKEN*/)->type == lcSEMI) {
    get_token(/*NEXT_TOKEN*/);
  } else {
    exptected_func("SEMI");
  } 
}

void do_abort(node_t **root) {
  node_t *abort_node = *root = create_node(curr_token->type, strdup(curr_token->text));
  if (get_token(/*NEXT_TOKEN*/)->type == lcSEMI) {
    get_token(/*NEXT_TOKEN*/);
  } else {
    exptected_func("SEMI");
  } 
}

void do_return(node_t **root) {
  node_t *return_node = *root = create_node(curr_token->type, strdup(curr_token->text));
  get_token();
  assignment_expression(&return_node->left);
  if (curr_token->type == lcSEMI) {
    get_token(/*NEXT_TOKEN*/);
  } else {
    exptected_func("SEMI");
  }

}

way_out statement(node_t **root) {
  int end_block = FALSE;      /* End of current block of statements */
  way_out out = NORMAL;
  node_t *statements = *root; /* List of statements */ 
  node_t *curr_statement;     /* Current recognized statement */
  DEBUG_TRACE("in stmnt\n");
  while (!end_block) {
    switch (curr_token->type) {
    case lcIF: {
      do_if(&curr_statement);
    } break;
    case lcWHILE: {
      do_while(&curr_statement);
    } break;
    case lcBREAK: {
      do_break(&curr_statement);
    } break;
    case lcCONTINUE: {
      do_continue(&curr_statement); 
    } break;
    case lcRETURN: {
      do_return(&curr_statement); 
    } break;
    case lcLBRACKET: {
      out = compound_statement(&statements);
      *root = statements;
      DEBUG_TRACE("after comp tok = %s\n", statements->text);
      //curr_statement = statements;
      if (curr_token->type != lcRBRACKET) {
        ERROR("error: expected }\n");
      }
      end_block = TRUE;
    } break;
    case lcPRINT: {
      do_print(&curr_statement);
      if ((curr_token = curr_token)->type == lcSEMI) {
        get_token(/*NEXT_TOKEN*/);
      }
    } break;
    case lcREAD: {
      do_read();

    } break;
    case lcABORT: {
      do_abort(&curr_statement);
    } break;
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
    } break;
    case lcINTERPRETE: {
      do_interprete();
    } break;
    case lcIDENT: {
    case lcNUMBER: 
      curr_statement = create_node(lcEXP, "expression");
      DEBUG_ALL("created expression");
      curr_statement->right = eval();
      if (curr_token->type != lcSEMI) {
        exptected_func("SEMI");
      }
      get_token(/*NEXT_TOKEN*/);
    } break;
    case lcVAR: {
      
      define_var(&curr_statement);
      if (curr_token->type == lcSEMI) {
        get_token();
      }
    } break;
    default: {

      /* 
      node_t *tmp;
      puts("**********");
      puts("Begin statement list");
      for (tmp = (*root); tmp->right != NULL; tmp = tmp->left)
      {
        assert(tmp != NULL);
        //assert(tmp->text != NULL);
        printf("\t-%s\n", tmp->right->text);
      }
      puts("**********");
      puts("End statement list");
      */
      out = NORMAL;
      end_block = TRUE;
    } break;
    }
    //assert(curr_statement != NULL);
    if (!end_block) {
      statements->right = curr_statement;
      statements->left = create_node(lcSTMNT, "statement");
      statements  = statements->left;
    }

  }
  return out;
}

way_out compound_statement(node_t **root) {
  way_out out = NORMAL;
  token_t prev_token;
  node_t *block;
  DEBUG_TRACE("in function %s\n", __FUNCTION__);
  memcpy(&prev_token, curr_token, sizeof(token_t));
  if (curr_token->type == lcLBRACKET) {
    block = create_node(lcBLOCK, "block");
    //printf("created block\n");
    /**************************************/
    get_token();
    block->right=create_node(lcSTMNT, "statement");
    out = statement(&block->right);
    //printf("block right = %s\n", block->right->right->text);
    if (curr_token->type == lcRBRACKET) {
      //puts("End compound!");
      //get_token();
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
  function = *root;
    DEBUG_TRACE("Curr func name = (%s)\n\n", function->text);
    declaration_list(&arg_list);
    out = compound_statement(&block);
    
    node_t *tmp;
    assert(block != NULL);
    /*
    puts("**********");
    puts("Begin statement list");
    printf("[%s]\n", block->text);
    print_statements(block, 0); 
    puts("**********");
    puts("End statement list");
    */

  function->left = arg_list;
  function->right = block;
  //*root = function;
  DEBUG_TRACE("\nend function = (%s)", (*root)->text);
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

int define_var(node_t **root){
  int res = 0;
  int is_get_tok = 1;
  token_type type;
  char *varname = "";
  node_t *value = NULL; 
  if (get_token()->type == lcIDENT)
  {
    varname = strdup(curr_token->text);
    printf("varname = %s\n", varname);
    *root = create_node(lcVARDEF, "var_assign");
    node_t *value;
    if (get_token()->type == lcASSIGN)
    {
      if ((type = get_token()->type) == lcIDENT || type == lcNUMBER || type == lcSTRING)
      {
        value = eval();
        assert(value != NULL);
        value->text = varname;
        assign_value(value);
      } 
    }
    else { 
      value = create_node(curr_token->type, varname);
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
    get_token();
  return res;
}

void var_definition(node_t **root) {
  //while(get_token()->type != lcSEMI);
}
