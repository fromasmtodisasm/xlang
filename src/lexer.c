/***************************************************************************/
/****************************** Includes ***********************************/
/***************************************************************************/
#include <assert.h>
#include <ctype.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "lexer.h"
#include "common.h"


/***************************************************************************/
/***************** Defines used by this module only ************************/
/***************************************************************************/
#define CURTOK() (curr_context->curr_token)
#define NUMTOKS 2

/***************************************************************************/
/********** Global Variables defined for this module only ******************/
/************************ (should be static) *******************************/
/***************************************************************************/
context_t context;
context_t *curr_context;
token_t *curr_token;
static char *source;
struct commands { 
  char command[20];
  token_type tok;
} table[] = {
    "if",       lcIF,
    "else",     lcELSE,     "for",        lcFOR,        "do",       lcDO,
    "while",    lcWHILE,    "char",       lcCHAR,       "int",      lcINT,
    "return",   lcRETURN,   "quit",       lcABORT,      "print",    lcPRINT,
    "read",     lcREAD,     "break",      lcBREAK,      "goto",     lcGOTO,
    "continue", lcCONTINUE, "begin",      lcBEGINBLOCK, "function", lcFUNCTION,
    "void",     lcVOID,     "interprete", lcINTERPRETE, "sleep",    lcSLEEP,
    "end",      lcENDBLOCK, "var",         lcVAR,        
    "pause",    lcPAUSE,    "",        lcEND
};

int jump_statement[] = {
    lcGOTO,
    lcCONTINUE,
    lcBREAK,
    lcRETURN,
};

char *token_to_string[] = {"lcEND",
                           "lcDELIM",
                           "lcSEMI",
                           "lcCOMMENT",
                           "lcKEYWORD",
                           "lcIDENT",
                           "lcNUMBER",
                           "lcSTRING",
                           "lcPLUS",
                           "lcMINUS",
                           "lcMUL",
                           "lcDIV",
                           "lcASSIGN",
                           "lcPLUS_ASSIGN",
                           "lcMINUS_ASSIGN",
                           "lcMUL_ASSIGN",
                           "lcDIV_ASSIGN",
                           "lcEQ",
                           "lcLBRACE",
                           "lcRBRACE",
                           "lcBREAK"
                           "lcIF",
                           "lcELSE",
                           "lcFOR",
                           "lcDO",
                           "lcWHILE",
                           "lcCHAR",
                           "lcINT",
                           "lcRETURN",
                           "lcLBRACKET",
                           "lcRBRACKET",
                           "lcABORT",
                           "lcPRINT",
                           "lcUNKNOWN"};

/***************************************************************************/
/*********************** Function Prototypes *******************************/
/******  (should be static, if not they should be in '.h' file) ************/
/***************************************************************************/

token_type is_keyword(char *name) {
  token_type type = lcEND;
  int i;
  for (i = 0; table[i].tok != lcEND; i++) {
    if (!strcmp(name, table[i].command)) {
      type = table[i].tok;
      break;
    }
  }
  return type;
}

int lexerInit(char *src) {
  curr_context = &context;
  curr_context->pos = src;
  curr_context->cur_line = 1;
  curr_token = &(curr_context->curr_token);

  return 1;
}

char *get_pos() { return curr_context->pos; }

void set_pos(char *pos) { curr_context->pos = pos; }

int get_line() { return curr_context->cur_line; }

token_t *get_token() {
  char *pos = curr_context->pos;
  token_type type = lcEND;
  void *value = NULL;
  char *text = NULL;
  char *begin = pos;
  char curr_digit[NUMBER_LEN];
  char curr_ident[IDENT_LEN];
  //static char curr_oper[3] = {0};

  if (*pos == '\0') {
    printf("End of source\n");
  }
  while (*pos){
    if (*pos == ' ' || *pos == '\t' || *pos == '\r' || *pos == '\n') {
      if (*pos == '\n') {
        curr_context->cur_line++;
      }
      pos++;
    }
    else if (*pos == '#') {
      pos++;
      while (*pos) {
        if (*pos == '\n') {
          type = lcCOMMENT;
          curr_context->cur_line++;
          pos++;
          break;
        }
        pos++;
      }
    }
    else if (*pos == '/' && (*(pos + 1) == '/' || *(pos + 1) == '*')) {
      if (*(pos + 1) == '/') {
        pos += 2;
        while (*pos) {
          if (*pos == '\n') {
            type = lcCOMMENT;
            pos++;
            break;
          }
          pos++;
        }
      } else if (*(pos + 1) == '*') {
        pos += 2;
        while (*pos) {
          if (*pos == '*' && *(pos + 1) == '/') {
            type = lcCOMMENT;
            pos += 2;
            break;
          }
          pos++;
        }
      }
      curr_context->cur_line++;
    }
    else break;
  } 
  begin = pos;

  if (*pos == '\0') {
    CURTOK().type = type = lcEND;
  }

  /*
   * Parse identifier
   */

  else if (isalpha(*pos) || *pos == '_') {
    int len = 0;
	token_type tmp;
    while (isalpha(*pos) || *pos == '_' || isdigit(*pos)) {
      if (len < IDENT_LEN)
        curr_ident[len++] = *pos++;
    }
    curr_ident[len] = 0;

    text = strdup(curr_ident);
    
    if ((tmp = is_keyword(text)) != lcEND) {
      type = tmp;
    } else {
      type = lcIDENT;
    }

  }
  /*
   * Parse identifier
   */
  else if ((isdigit(*pos) || *pos == '.') && !isalpha(pos[1])) {
    int val = 0;
    int hex_val = 0;
    int radix = 10;
    if (*pos == '0') {
      if (*(pos + 1) == 'x' || *(pos + 1) == 'X') {
        radix = 16;
        pos += 2;
        while (*pos) {
          if (*pos >= '0' && *pos <= '9') {
            // val = val * radix + (*pos - '0');
            pos++;
          } else if (*pos >= 'A' && *pos <= 'F' || *pos >= 'a' && *pos <= 'f') {
            if (*pos >= 'a') {
              // hex_val = *pos - 'A' - 32 + 10;
            } else {
              // hex_val = *pos - 'A' + 10;
            }
            // val = val * radix + (hex_val);
            pos++;
          } else {
            break;
          }
        }
      } else {
        radix = 8;
        pos++;
        while (*pos) {
          if (isdigit(*pos)) {
            val = val * radix + (*pos - '0');
            pos++;
          } else {
            break;
          }
        }
      }
    } else {
      char *digit = curr_digit;
      while (*pos) {
        if (isdigit(*pos) || *pos == '.') {
          val = val * 10 + (*pos - '0');
          *digit = *pos++;
          digit++;
        } else {
          *digit = 0;
          break;
        }
      }
    }

    text = strdup(curr_digit);
    type = lcNUMBER;
  } else if (*pos == ',') {
    type = lcCOMMA;
    pos++;
  } else if (*pos == ';') {
    type = lcSEMI;
    pos++;
  } else if (*pos == '(') {
    type = lcLBRACE;
    pos++;
  } else if (*pos == ')') {
    type = lcRBRACE;
    pos++;
  } else if (*pos == '{') {
    type = lcLBRACKET;
    pos++;
  } else if (*pos == '}') {
    type = lcRBRACKET;
    pos++;
  } else if (*pos == '[') {
    type = (token_type)*pos;
    pos++;
  } else if (*pos == ']') {
    type = (token_type)*pos;
    pos++;
  } else if (*pos == '.') {
    type = lcPOINT;
    pos++;
  } else if (*pos == '\"') {
    int len = 0;
    pos++;
    while (*pos) {
      if (*pos == '\"') {
        pos++;
        break;
      }
      len++;
      pos++;
    }

    text = (char*)malloc(len + 1);
    memcpy(text, pos - len - 1, len);
    text[len] = '\0';
    type = lcSTRING;
  } else if (type == lcEND) {
    switch (*pos) {
    case '+': {
      if (pos[1] == '=') {
        pos++;
        type = lcPLUS_ASSIGN;
      } else if (pos[1] == '+') {
        type = lcPLUS_PLUS; 
        pos++;
      } else {
        type = lcPLUS;
      }
      break;
    }
    case '-': {
      if (pos[1] == '=') {
        type = lcMINUS_ASSIGN;
        pos++;
      } else if (pos[1] == '-') {
        type = lcMINUS_MINUS;
        pos++;
      } else if (pos[1] == '>') {
        type = lcARROW;
        pos++;
      } else {
        type = lcMINUS;
      }
      break;
    }
    case '*': {
      if (pos[1] == '=') {
        pos++;
        type = lcMUL_ASSIGN;
      } else {
        type = lcMUL;
      }
      break;
    }
    case '/': {
      if (pos[1] == '=') {
        pos++;
        type = lcDIV_ASSIGN;
      } else {
        type = lcDIV;
      }
      break;
    }
    case '=': {
      if (pos[1] == '=') {
        type = lcEQ_OP;
        pos++;
      } else {
        type = lcASSIGN;
      }
      break;
    }
    case '<': {
      if (pos[1] == '=') {
        type = lcLE_OP;
        pos++;
      } else {
        type = lcL_OP;
      }
      break;
    }
    case '>': {
      if (pos[1] == '=') {
        type = lcGE_OP;
        pos++;
      } else {
        type = lcG_OP;
      }
      break;
    }
    case '!': {
      if (pos[1] == '=') {
        type = lcNE_OP;
        pos++;
      }
      break;
    }
    case '&': {
      if (pos[1] == '&') {
        type = lcAND_OP;
        pos++;
      } else {
        type = lcUNKNOWN;
      }
      break;
    }
    case '|': {
      if (pos[1] == '|') {
        type = lcAND_OP;
        pos++;
      } else {
        type = lcUNKNOWN;
      }
      break;
    }
    }
    if (type != lcEND) {
      pos++;
    }
  } else if (*pos == '\0') {
    type = lcEND;
  } else {
    ERROR("UNKNOWN token\n");
    type = lcUNKNOWN;
    pos++;
  }
  if (!text && ((type != lcEND) && (type != lcUNKNOWN))) {

    assert(type != lcEND);
    assert(pos > begin);
    text = strndup(begin, pos - begin);
    //text[pos-begin] = '\0';
  }
  CURTOK().type = type;
  CURTOK().text = text;
  CURTOK().pos = begin;
  curr_context->pos = pos;
  DEBUG_DEVELOP("< %s, %s >\n", "text" /*token_to_string[CURTOK().type - BASE_INDEX]*/, text);
  assert(&CURTOK() != NULL);
  if (type == lcEND) DEBUG_TRACE("End of source\n");
  return &CURTOK();
}
