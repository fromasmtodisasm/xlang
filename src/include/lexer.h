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
//#define CURRENT_TOKEN() (curr_context->CURRENT_TOKEN)
#define DEFINE_LEXER_CONTEXT() //static lexer_context_t* curr_context

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
	lcIFELSE,
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

	lcUNIT,
	lcEXP,
	lcSTMNT,

  lcSTRUCT,

	lcUNKNOWN
} token_type;
typedef enum token_category {
	catIDENT,
	catKEYWORD,
	catCONST,
	catOPERATOR,
	catDELIM
} token_category;

typedef struct string_ref
{
    char* pos;
    int len;
}string_ref;

void string_ref_assign(string_ref* ref, char* str);

typedef struct token_t {
	token_type type;
	string_ref text;
	token_category category;
	char *pos;
} token_t;
typedef struct lexer_context_t lexer_context_t;
string_ref string_ref_create(char* str);

/***************************************************************************/
/*************************** Prototypes ************************************/
/***************************************************************************/
lexer_context_t* lexer_create_context();
token_type is_keyword(string_ref name);
char *lexer_get_pos(lexer_context_t*);
void lexer_set_pos(lexer_context_t*, char *pos);
void lexer_set_token(lexer_context_t* ctx, token_t* token);
int get_line(lexer_context_t*);
token_t *get_token(lexer_context_t*);
token_t CURRENT_TOKEN(lexer_context_t*);
void lexer_update_context(lexer_context_t** dst, lexer_context_t* src);
void lexer_save_context(lexer_context_t** dst, lexer_context_t* src);
lexer_context_t* get_context();
void set_context(lexer_context_t* );
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
//extern token_t *CURRENT_TOKEN;
//extern lexer_context_t* current_context;

#endif /* _LEXER_H_ */