#pragma once

#ifdef __cplusplus
#include "token_struct.hpp"
#include <string>

extern Token yylval;

#endif


#define TOK_PLUS     258
#define TOK_MINUS    259
#define TOK_STAR     260
#define TOK_SLASH    261
#define TOK_LPAREN   262
#define TOK_RPAREN   263
#define TOK_NUMBER   264
#define TOK_VAR      265
#define TOK_END_FILE 266
#define TOK_ERROR    -1
