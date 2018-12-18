#ifndef _LEXER_H_
#define _LEXER_H_

/***************************************************************************/
/************************** Other Includes *********************************/
/***************************************************************************/

/***************************************************************************/
/***************************** Defines *************************************/
/***************************************************************************/
#define BASE_INDEX 257
#define IDENT_LEN 64
#define NUMBER_LEN 20

/***************************************************************************/
/*********************** Typedefs/Structures *******************************/
/***************************************************************************/
typedef enum token_direct { PREV, CURR, NEXT } token_direct;
typedef enum token_type {
  lcEND = BASE_INDEX,
  lcDELIM,
  lcSEMI,
  lcCOMMENT,
  lcKEYWORD,
  lcIDENT,
  lcNUMBER,
  lcSTRING,
  lcPLUS,
  lcMINUS,
  lcMUL,
  lcDIV,
  lcASSIGN,
  lcPLUS_ASSIGN,
  lcPLUS_PLUS,
  lcMINUS_ASSIGN,
  lcMINUS_MINUS,
  lcMUL_ASSIGN,
  lcDIV_ASSIGN,

  lcCOMMA,

  lcINC_OP,
  lcDEC_OP,
  lcPTR_OP,
  lcAND_OP,
  lcOR_OP,
  lcLE_OP,
  lcGE_OP,
  lcEQ_OP,
  lcNE_OP,

  lcG_OP,
  lcN_OP,
  lcL_OP,

  lcGOTO,
  lcCONTINUE,
  lcBREAK,
  lcRETURN,

  lcLBRACE,
  lcRBRACE,

  lcIF,
  lcELSE,
  lcFOR,
  lcDO,
  lcWHILE,
  lcCHAR,
  lcINT,
  lcVAR,

  lcLBRACKET,
  lcRBRACKET,
  lcABORT,
  lcPRINT,
  lcREAD,
  lcSLEEP,

  lcBEGINBLOCK,
  lcENDBLOCK,
  lcFUNCTION,
  lcINTERPRETE,

  lcVOID,
  lcPAUSE,

  lcPOINT,
  lcARROW,

  lcCALL,
  lcBLOCK,
  lcSTATEMENT,
  lcVARDEF,

  lcUNKNOWN
} token_type;
typedef enum token_category {
  catIDENT,
  catKEYWORD,
  catCONST,
  catOPERATOR,
  catDELIM
} token_category;

typedef struct token_t {
  token_type type;
  char *text;
  token_category category;
  char *pos;
} token_t;
typedef struct context {
  token_t curr_token;
  char *pos;
  int cur_line;
  struct context *prev, *next;
} context_t;

/***************************************************************************/
/*************************** Prototypes ************************************/
/***************************************************************************/
token_type is_keyword(char *name);
int lexerInit(char *src);
char *get_pos();
void set_pos(char *pos);
int get_line();
token_t *get_token();
/*
union
{
int int_number;
char *string;
float float_number
}value;
*/

/***************************************************************************/
/********************* Externally Defined Globals **************************/
/***************************************************************************/
extern token_t *curr_token;

#endif /* _LEXER_H_ */
