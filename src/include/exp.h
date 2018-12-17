#ifndef _EXP_H_ /* Protect against multiple inclusion */
#define _EXP_H_ 
/***************************************************************************/
/************************** Other Includes *********************************/
/***************************************************************************/
#include "lexer.h"
#include "tree.h"

/***************************************************************************/
/***************************** Defines *************************************/
/***************************************************************************/

/***************************************************************************/
/*********************** Typedefs/Structures *******************************/
/***************************************************************************/
typedef enum variable_type {
  VAR_INT,
  VAR_STRING,
  VAR_BOOL,
  VAR_CONST,
  VAR_SONST_LITERAL
} variable_type;

typedef struct variable {
  char *name;
  float value;
  int type;
  struct variable *next;
} variable;

/***************************************************************************/
/*************************** Prototypes ************************************/
/***************************************************************************/
int block(char **buffer);
int assignment_expression(node_t **node);
int lookup(node_t *node);
float assign_value(node_t *node);
int primary_expression(node_t **node);
int multiplicative_expression(node_t **node);
int additive_expression(node_t **node);
int conditional_expression(node_t **node);

int exp_parser_init();
void prefix_tree(node_t *tree, int level);
node_t *eval();

/***************************************************************************/
/********************* Externally Defined Globals **************************/
/***************************************************************************/

#endif //_EXP_H_
