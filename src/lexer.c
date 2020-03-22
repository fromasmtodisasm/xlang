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
#define NUMTOKS 2
#define ERROR(A, ...)
#define DEBUG_TRACE(A, ...)

/***************************************************************************/
/*********************** Typedefs/Structures *******************************/
/***************************************************************************/
typedef struct lexer_context_t{
  token_t current_token;
  char *pos;
  int line;
} lexer_context_t;

/***************************************************************************/
/********** Global Variables defined for this module only ******************/
/************************ (should be static) *******************************/
/***************************************************************************/
lexer_context_t context;
DEFINE_LEXER_CONTEXT();
//token_t *CURRENT_TOKEN;
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
  "end",      lcENDBLOCK, "var",        lcVAR,
  "pause",    lcPAUSE,    "struct",     lcSTRUCT,      "",        lcEND
};

int jump_statement[] = {
  lcGOTO,
  lcCONTINUE,
  lcBREAK,
  lcRETURN,
};

char *token_to_string[] = { "lcEND",
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
               "lcUNKNOWN" };

/***************************************************************************/
/*********************** Function Prototypes *******************************/
/******  (should be static, if not they should be in '.h' file) ************/
/***************************************************************************/
static char* strndup(const char* str, size_t n) {
	char* out;
	size_t len = strlen(str);
	if (len > n) len = n;
	out = MALLOC(char, len);
	strncpy(out, str, len);
	return out;
}

string_ref string_ref_create(char* str)
{
	string_ref result;
	result.pos = str;
	result.len = strlen(str);
	return result;
}

void string_ref_assign(string_ref* ref, char* str)
{
	ref->pos = str;
	ref->len = strlen(str);
}

void token_init(token_t* t)
{
	memset(t, 0, sizeof(token_t));
}

lexer_context_t* lexer_create_context()
{
	lexer_context_t* ctx = MALLOC(lexer_context_t, sizeof(context));
	if (ctx != NULL)
	{
		token_init(&ctx->current_token);
		ctx->line = 1;
		ctx->pos = NULL;
	}
	return ctx;
}

token_type is_keyword(string_ref name) {
	token_type type = lcEND;
	int i;
	for (i = 0; table[i].tok != lcEND; i++) {
		if (!strncmp(name.pos, table[i].command, strlen(table[i].command))) {
			type = table[i].tok;
			break;
		}
	}
	return type;
}

char* lexer_get_pos(lexer_context_t* ctx) { return ctx->pos; }

void lexer_set_pos(lexer_context_t* ctx, char* pos) { ctx->pos = pos; }

void lexer_set_token(lexer_context_t* ctx, token_t* token)
{
	ctx->current_token = *token;
}

int lexer_get_line(lexer_context_t* ctx) { return ctx->line; }

token_t* lexer_get_token(lexer_context_t* ctx) {
	char* pos = ctx->pos;
	token_type type = lcEND;
	void* value = NULL;
	string_ref text = { .pos = NULL, .len = 0 };
	char* begin = pos;
	char curr_digit[NUMBER_LEN];
	//char curr_ident[IDENT_LEN];
	//static char curr_oper[3] = {0};

	while (*pos) {
		if (*pos == ' ' || *pos == '\t' || *pos == '\r' || *pos == '\n') {
			if (*pos == '\n') {
				ctx->line++;
			}
			pos++;
		}
		else if (*pos == '#') {
			pos++;
			while (*pos) {
				if (*pos == '\n') {
					type = lcCOMMENT;
					ctx->line++;
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
			}
			else if (*(pos + 1) == '*') {
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
			ctx->line++;
		}
		else break;
	}
	begin = pos;

	switch (*pos)
	{

	case '\"': {
		text.len++;
		while (pos[text.len]) {
			if (pos[text.len] == '\"') {
				text.len++;
				break;
			}
			text.len++;
		}

		text.pos = pos;
		pos += text.len;
		type = lcSTRING;
		break;
	}
	case ',': {
		type = lcCOMMA;
		pos++;
		break;
	}
	case ';': {
		type = lcSEMI;
		pos++;
		break;
	}
	case '(': {
		type = lcLBRACE;
		pos++;
		break;
	}
	case ')': {
		type = lcRBRACE;
		pos++;
		break;
	}
	case '{': {
		type = lcLBRACKET;
		pos++;
		break;
	}
	case '}': {
		type = lcRBRACKET;
		pos++;
		break;
	}
	case '[': {
		type = (token_type)*pos;
		pos++;
		break;
	}
	case ']': {
		type = (token_type)*pos;
		pos++;
		break;
	}
	case '.': {
		type = lcPOINT;
		pos++;
		break;
	}
	case '+': {
		if (pos[1] == '=') {
			pos++;
			type = lcPLUS_ASSIGN;
		}
		else if (pos[1] == '+') {
			type = lcPLUS_PLUS;
			pos++;
		}
		else {
			type = lcPLUS;
		}
		break;
	}
	case '-': {
		if (pos[1] == '=') {
			type = lcMINUS_ASSIGN;
			pos++;
		}
		else if (pos[1] == '-') {
			type = lcMINUS_MINUS;
			pos++;
		}
		else if (pos[1] == '>') {
			type = lcARROW;
			pos++;
		}
		else {
			type = lcMINUS;
		}
		break;
	}
	case '*': {
		if (pos[1] == '=') {
			pos++;
			type = lcMUL_ASSIGN;
		}
		else {
			type = lcMUL;
		}
		break;
	}
	case '/': {
		if (pos[1] == '=') {
			pos++;
			type = lcDIV_ASSIGN;
		}
		else {
			type = lcDIV;
		}
		break;
	}
	case '=': {
		if (pos[1] == '=') {
			type = lcEQ_OP;
			pos++;
		}
		else {
			type = lcASSIGN;
		}
		break;
	}
	case '<': {
		if (pos[1] == '=') {
			type = lcLE_OP;
			pos++;
		}
		else {
			type = lcL_OP;
		}
		break;
	}
	case '>': {
		if (pos[1] == '=') {
			type = lcGE_OP;
			pos++;
		}
		else {
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
		}
		else {
			type = lcUNKNOWN;
		}
		break;
	}
	case '|': {
		if (pos[1] == '|') {
			type = lcAND_OP;
			pos++;
		}
		else {
			type = lcUNKNOWN;
		}
		break;
	}
	case '\0': {
		ctx->current_token.type = type = lcEND;
		break;
	}
	default:
	{
		/*
		 * Parse identifier
		 */
		if (isalpha(*pos) || *pos == '_') {
			token_type tmp;
			while (isalpha(pos[text.len]) || pos[text.len] == '_' || isdigit(pos[text.len])) {
				text.len++;
			}

			text.pos = pos;
			pos += text.len;

			if ((tmp = is_keyword(text)) != lcEND) {
				type = tmp;
			}
			else {
				type = lcIDENT;
			}

		}
		/*
		 * Parse number
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
						}
						else if (*pos >= 'A' && *pos <= 'F' || *pos >= 'a' && *pos <= 'f') {
							if (*pos >= 'a') {
								// hex_val = *pos - 'A' - 32 + 10;
							}
							else {
								// hex_val = *pos - 'A' + 10;
							}
							// val = val * radix + (hex_val);
							pos++;
						}
						else {
							break;
						}
					}
				}
				else {
					radix = 8;
					pos++;
					while (*pos) {
						if (isdigit(*pos)) {
							val = val * radix + (*pos - '0');
							pos++;
						}
						else {
							break;
						}
					}
				}
			}
			else {
				char* digit = curr_digit;
				while (*pos) {
					if (isdigit(*pos) || *pos == '.') {
						val = val * 10 + (*pos - '0');
						text.len++;
					}
					else {
						break;
					}
				}
			}

			text.pos = curr_digit;
			type = lcNUMBER;
		}
		else {
			ERROR("UNKNOWN token\n");
			type = lcUNKNOWN;
			pos++;
		}
	}
	break;
	}


	if (!text.len && ((type != lcEND) && (type != lcUNKNOWN))) {

		assert(type != lcEND);
		assert(pos > begin);
	}
	ctx->current_token.type = type;
	ctx->current_token.text = text;
	ctx->current_token.pos = begin;
	ctx->pos = pos;
	//DEBUG_DEVELOP("< %s, %s >\n", "text" /*token_to_string[CURRENT_TOKEN().type - BASE_INDEX]*/, text);
	if (type == lcEND) DEBUG_TRACE("End of source\n");
	return &ctx->current_token;
}

token_t CURRENT_TOKEN(lexer_context_t* ctx)
{
	return ctx->current_token;
}

void lexer_update_context(lexer_context_t** dst, lexer_context_t* src)
{
	**dst = *src;
}
void lexer_save_context(lexer_context_t** dst, lexer_context_t* src)
{
	*dst = MALLOC(lexer_context_t, sizeof(lexer_context_t));
	**dst = *src;
}
#if 0
lexer_context_t* get_context()
{
	return curr_context;
}

void set_context(lexer_context_t* ctx)
{
	curr_context = ctx;
}
#endif

