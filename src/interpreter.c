#include "interpreter.h"
#include "common.h"
#include "preprocessor.h"

#include <memory.h> //memcpy
#include <stdio.h>
#include <unistd.h> //sleep

#define exptected_func(...) printf("On line %d\n", __LINE__);_expected_func( __VA_ARGS__)

extern char *token_to_string[];
void skip_compound_statement();
void skip_statement();
int function_definition();
int declaration_list();

static int is_type(token_type type);

token_type eat_tokens(token_type skip_to);

void _expected_func(char *exptected) {
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

way_out do_if(node_t *root) {
  int condition = 0;
  way_out out = NORMAL;

  get_token(/*NEXT_TOKEN*/);
  if (curr_token->type == lcLBRACE) {
    get_token(/*NEXT_TOKEN*/);
    condition = (int)eval();
    if ((curr_token = curr_token)->type == lcRBRACE) {
      if (condition) {
        get_token(/*NEXT_TOKEN*/);
        if ((out = statement(SELECTION)) == CONTINUE || out == BREAK) {
          // get_token()/*FIXME: change process getting <PREV_TOKEN>*/;
        } else if (get_token(/*NEXT_TOKEN*/)->type == lcELSE) {
          get_token(/*NEXT_TOKEN*/);
          skip_statement();
        }
      } else {
        get_token(/*NEXT_TOKEN*/);
        skip_statement();
        if (get_token(/*NEXT_TOKEN*/)->type == lcELSE) {
          get_token(/*NEXT_TOKEN*/);
          out = statement(SELECTION);
        }
      }
    }
  }
  ///*get_token(/*NEXT_TOKEN*/);*/
  return out;
}

way_out do_while(node_t *root) {
  int condition = 0;
  way_out out = NORMAL;

  get_token(/*NEXT_TOKEN*/);
  if (curr_token->type == lcLBRACE) {
    char *pos_begin = curr_token->pos;
    char *pos_end = pos_begin;

    while (get_token(/*NEXT_TOKEN*/), condition = (int)eval()) {
      if ((curr_token = curr_token)->type == lcRBRACE) {
        get_token(/*NEXT_TOKEN*/);
        if ((out = statement(ITERATION)) == BREAK) {
          break;
        } else if (out == CONTINUE || out == NORMAL) {
          pos_end = curr_token->pos;
          set_pos(pos_begin);
          get_token(/*NEXT_TOKEN*/);
          continue;
        } else if (out == RETURN) {
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

int func_decl() { return 0; }
/*!
   \brief Это функция очень великая

*/
int start(char **buffer) {
  int retval = 0;

  exp_parser_init();

  if ((lexerInit(*buffer)) != 0) {
    while (get_token(/*NEXT_TOKEN*/)->type != lcEND) {
      retval = function_definition();
    }
  }

  return retval;
}

int is_print() { return 0; }

int print(node_t *root) {
  int stop = 0;
  get_token(/*NEXT_TOKEN*/);
  do {
    char *number = "%f";
    char *string = "%s";
    char *curtype;
    float expr_val = 0;
    switch (curr_token->type) {
    case lcSTRING:
      printf("%s", curr_token->text);
      get_token(/*NEXT_TOKEN*/);
      break;

    case lcNUMBER:
    case lcIDENT:
      curtype = number;
      expr_val = eval();
      printf("%f", expr_val);
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
      assign_value(curr_token->text, tmp);
    }
  }
}

void do_sleep() {
  int ms;
  get_token();
  ms = eval();
  sleep(ms);
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

  node_t *root;

  while (TRUE) {
    switch (curr_token->type) {
    case lcIF: {
      if ((out = do_if(root)) == CONTINUE || out == BREAK)
        return out;
    } break;
    case lcWHILE: {
      do_while(root);
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
      print(root);
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
    case lcINTERPRETE: {
      interprete();
    } break;
    case lcIDENT:
    case lcNUMBER: {
      res = eval();
      if (curr_token->type != lcSEMI) {
        exptected_func("SEMI");
        goto abort;
      }
      // printf("exp evaluated\n");
      get_token(/*NEXT_TOKEN*/);
    } break;
    case lcVAR: {
      
      define_var();
      if (curr_token->type == lcSEMI) {
        get_token();
      }
    } break;
    default:
      return NORMAL;
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

  memcpy(&prev_token, curr_token, sizeof(token_t));
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

int function_definition() {
  way_out out;
  token_type type = curr_token->type;
  if (is_type(type) && get_token(/*NEXT_TOKEN*/)->type == lcIDENT) {
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
  
  if (get_token()->type == lcIDENT)
  {
    varname = strdup(curr_token->text);
    if (get_token()->type == lcASSIGN)
    {
      if ((type = get_token()->type) == lcIDENT || type == lcNUMBER)
      {
        float value = eval();
        assign_value(varname, value);
      } 
    }
    else { 
      assign_value(varname, 0);
      is_get_tok = FALSE; 
    }
    res = TRUE;
  }
  else
  {
    fprintf(stderr, "Expected identifier\n");
  }
  if (is_get_tok)
    get_token();
  return res;
}
