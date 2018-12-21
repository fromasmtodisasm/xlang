#ifndef _PARSER_H_  /* Protect against multiple inclusion */
#define _PARSER_H_ 

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************/
/************************** Other Includes *********************************/
/***************************************************************************/
#include "exp.h"

/***************************************************************************/
/*********************** Typedefs/Structures *******************************/
/***************************************************************************/
typedef enum compound_origin { COMPOUND, ITERATION, SELECTION } compound_origin;

typedef enum way_out { NORMAL, BREAK, CONTINUE, RETURN } way_out;

/***************************************************************************/
/*************************** Prototypes ************************************/
/***************************************************************************/
node_t *parse(char **buffer);
way_out do_if();
way_out do_while();
way_out statement(node_t **root);
way_out compound_statement(node_t **root);
int define_var(node_t **root);

/***************************************************************************/
/********************* Externally Defined Globals **************************/
/***************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _INTERPRETER_H_ */
