#pragma once

#ifdef __cplusplus
extern "C" {
#endif
// __cplusplus

typedef enum compound_origin { COMPOUND, ITERATION, SELECTION } compound_origin;

typedef enum way_out { NORMAL, BREAK, CONTINUE, RETURN } way_out;

int start(char **buffer);
way_out do_if();
way_out do_while();
way_out statement(compound_origin origin);
way_out compound_statement(compound_origin origin);

#ifdef __cplusplus
}
#endif
