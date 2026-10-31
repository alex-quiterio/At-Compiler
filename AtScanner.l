%option c++ yyclass="AtScanner" outfile="AtScanner.cpp"
%option stack noyywrap yylineno 8bit
%{ 
#include <string>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <cdk/nodes/Node.h>
#include <cdk/nodes/Sequence.h>
#include <cdk/nodes/expressions/Expression.h>
#include "nodes/all.h"
#include "AtScanner.h"

// ensure that relevant includes are made before including the following
#include "AtParser.tab.h"
// this is terrible, but Flex's but C++ lexers are crap...
#define yyerror LexerError
// Parse an integer literal in the given base. Returns false when the value
// does not fit in an int (errno must be cleared first: strtol only sets it).
inline bool parse_int(const char *text, int base, int *value) {
  errno = 0;
  long v = strtol(text, NULL, base);
  *value = (int)v;
  return errno != ERANGE && v >= INT_MIN && v <= INT_MAX;
}
// Turn a number in engineering notation (e.g. "6k2") into "6.2". The caller
// applies the multiplier for the suffix letter.
inline double real_modify(char* number, int length) {
  for(int i = 0; i < length; i++)
  {
    if(isalpha(number[i]))
    {	
	number[i]='.';
	break;
    }
  }
    return strtod(number,NULL);
}
%}

%x X_COMMENT X_CHAR_STATE X_CHAR_SPECIAL X_STR_SPECIAL X_STR_STATE

%%


<INITIAL,X_COMMENT>"/*"		yy_push_state(X_COMMENT);
<X_COMMENT>"*/"			yy_pop_state();
<X_COMMENT>.|\n			; /* ignore comments */
"//".*$				; /* ignore comments */
">="				return tGE; 
"<="				return tLE; 
"=="				return tEQ; 
"!="				return tNE;
"||"				return tOR;
"&&"				return tAND;
"const"				return tCONST;
"void"				return tVOID;
"int"				return tINT;
"real"				return tFLOAT;
"string"			return tSTRING;
"public"			return tPUBLIC;
"use"				return tUSE;
"next"				return tNEXT;
"->"				return tFUNCTION;
"<<"				return tOPENBODY;
">>"				return tCLOSEBODY;
"stop"				return tSTOP;
"return"			return tRETURN;
"!!"				return tPRINT;

[_A-Za-z][A-Za-z0-9_]*  		{ yylval.s = new std::string(yytext); return tIDENTIFIER; }
[0][x][0]*[A-Fa-f1-9][A-Fa-f0-9]*	{ if (!parse_int(yytext, 16, &yylval.i)) yyerror("Value out of range!"); return tINTEGER; } /* hexadecimal */
[0][b][0]*[1][0-1]*			{ if (!parse_int(yytext + 2, 2, &yylval.i)) yyerror("Value out of range!"); return tINTEGER; } /* binary */
[0]+[1-7][0-7]*				{ if (!parse_int(yytext, 8, &yylval.i)) yyerror("Value out of range!"); return tINTEGER; } /* octal */
(0|[1-9][0-9]*)				{ if (!parse_int(yytext, 10, &yylval.i)) yyerror("Value out of range!"); return tINTEGER; } /* decimal */
[0-9]*"."[0-9]+				{yylval.r = strtod(yytext,NULL); return tREAL;}
[0-9]+"."[0-9]*				{yylval.r = strtod(yytext,NULL); return tREAL;}
[0-9]+[y][0-9]+				{yylval.r = real_modify(yytext, yyleng) * 1E-24; return tREAL;}
[0-9]+[z][0-9]+				{yylval.r = real_modify(yytext, yyleng) * 1E-21; return tREAL;}
[0-9]+[a][0-9]+				{yylval.r = real_modify(yytext, yyleng) * 1E-18; return tREAL;}
[0-9]+[f][0-9]+				{yylval.r = real_modify(yytext, yyleng) * 1E-15; return tREAL;}
[0-9]+[p][0-9]+				{yylval.r = real_modify(yytext, yyleng) * 1E-12; return tREAL;}
[0-9]+[n][0-9]+				{yylval.r = real_modify(yytext, yyleng) * 1E-9; return tREAL;}
[0-9]+[u][0-9]+				{yylval.r = real_modify(yytext, yyleng) * 1E-6; return tREAL;}
[0-9]+[m][0-9]+				{yylval.r = real_modify(yytext, yyleng) * 1E-3; return tREAL;}
[0-9]+[k][0-9]+				{yylval.r = real_modify(yytext, yyleng) * 1E3; return tREAL;}
[0-9]+[M][0-9]+				{yylval.r = real_modify(yytext, yyleng) * 1E6; return tREAL;}
[0-9]+[G][0-9]+				{yylval.r = real_modify(yytext, yyleng) * 1E9; return tREAL;}
[0-9]+[T][0-9]+				{yylval.r = real_modify(yytext, yyleng) * 1E12; return tREAL;}
[0-9]+[P][0-9]+				{yylval.r = real_modify(yytext, yyleng) * 1E15; return tREAL;}
[0-9]+[E][0-9]+				{yylval.r = real_modify(yytext, yyleng) * 1E18; return tREAL;}
[0-9]+[Z][0-9]+				{yylval.r = real_modify(yytext, yyleng) * 1E21; return tREAL;}
[0-9]+[Y][0-9]+				{yylval.r = real_modify(yytext, yyleng) * 1E24; return tREAL;}
[0-9]*"."[0-9]+[E][+-]?[0-9]+		{yylval.r = strtod(yytext,NULL); return tREAL;}
[-@()<>=^+*#~?/.%;{}!:,~]	   	return *yytext;
\[					return *yytext;
\]					return *yytext;
\'					{yy_push_state(X_CHAR_STATE); yylval.s = new std::string();}
<X_CHAR_STATE>\'			{yy_pop_state(); return tCHAR;}
<X_CHAR_STATE>\\			{yy_push_state(X_CHAR_SPECIAL);}
<X_CHAR_STATE>.\'			{yyless(yyleng-1); *yylval.s += YYText(); }
<X_CHAR_STATE>.|\n			yyerror("Invalid character");

<X_CHAR_SPECIAL>n\'			{yy_pop_state(); yyless(yyleng-1); *yylval.s += std::string(1,10); }
<X_CHAR_SPECIAL>r\'			{yy_pop_state(); yyless(yyleng-1); *yylval.s += std::string(1,13); }
<X_CHAR_SPECIAL>t\'			{yy_pop_state(); yyless(yyleng-1); *yylval.s += std::string(1,9); }
<X_CHAR_SPECIAL>[\'\\]\'		{yy_pop_state(); yyless(yyleng-1); *yylval.s += YYText(); }
<X_CHAR_SPECIAL>[0-9a-fA-F][0-9a-fA-F]?\' 	{yy_pop_state(); yyless(yyleng-1); *yylval.s += std::string(1,(char)strtol(YYText(),NULL,16)); }
<X_CHAR_SPECIAL>.|\n			yyerror("Invalid special character");

\"					{yy_push_state(X_STR_STATE); yylval.s = new std::string();}
<X_STR_STATE>\"				{yy_pop_state(); return tTEXT;}
<X_STR_STATE>\\				{yy_push_state(X_STR_SPECIAL);}
<X_STR_STATE>\0				yyerror("Invalid character in string (NUL)");
<X_STR_STATE>.				{*yylval.s += YYText();}

<X_STR_SPECIAL>n			{yy_pop_state(); *yylval.s += std::string(1,10); }
<X_STR_SPECIAL>r			{yy_pop_state(); *yylval.s += std::string(1,13); }
<X_STR_SPECIAL>t			{yy_pop_state(); *yylval.s += std::string(1,9); }
<X_STR_SPECIAL>[\"\\]			{yy_pop_state(); *yylval.s += std::string(1,*YYText()); }
<X_STR_SPECIAL>[0-9a-fA-F][0-9a-fA-F]?	{yy_pop_state(); *yylval.s += std::string(1,(char)strtol(YYText(),NULL,16)); }
<X_STR_SPECIAL>.|\n			yyerror("Invalid special character");
[ \t\n]+				; /* ignore whitespace */
.					yyerror("Unknown character");
%%
/**
 * Very, very dirty hack: flex is a mess generating C++.
 */
int AtScanner::yywrap() { return 1;}