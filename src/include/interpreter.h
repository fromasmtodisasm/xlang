#pragma once

#ifdef __cplusplus
extern "C" {
#endif 
// __cplusplus

typedef enum compound_origin
{
	COMPOUND,
	ITERATION,
	SELECTION
}compound_origin;

typedef enum way_out
{
	NORMAL,
	BREAK,
	CONTINUE,
	RETURN
}way_out;

way_out do_if();
int expr(char ** buffer);
way_out statement(compound_origin origin);
way_out compound_statement(compound_origin origin);

#ifdef __cplusplus
}
#endif
