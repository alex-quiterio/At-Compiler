%option c++ prefix="at_scanner_" outfile="at_scanner.cpp"
%option stack noyywrap yylineno 8bit
%{
// make relevant includes before including the parser's tab file
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <cctype>
#include <string>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>
#include "at_parser.tab.h"

// don't change this
#define yyerror LexerError

// Parse an integer literal in the given base. Returns false when the value
// does not fit in an int (errno must be cleared first: strtol only sets it).
static bool parse_int(const char *text, int base, int *value) {
  errno = 0;
  long v = strtol(text, nullptr, base);
  *value = (int)v;
  return errno != ERANGE && v >= INT_MIN && v <= INT_MAX;
}

// Engineering notation: "6k2" is 6.2 * 1E3. The suffix letter takes the
// place of the decimal point.
static double engineering(const char *text, double multiplier) {
  std::string number(text);
  for (char &c : number)
    if (isalpha((unsigned char)c)) { c = '.'; break; }
  return strtod(number.c_str(), nullptr) * multiplier;
}
%}

%x X_COMMENT X_CHAR X_CHAR_ESCAPE X_STRING X_STRING_ESCAPE

%%

<INITIAL,X_COMMENT>"/*"      yy_push_state(X_COMMENT);
<X_COMMENT>"*/"              yy_pop_state();
<X_COMMENT>.|\n              ; /* ignore comments */
"//".*$                      ; /* ignore comments */

">="                         return tGE;
"<="                         return tLE;
"=="                         return tEQ;
"!="                         return tNE;
"||"                         return tOR;
"&&"                         return tAND;
"->"                         return tFUNCTION;
"<<"                         return tOPENBODY;
">>"                         return tCLOSEBODY;
"!!"                         return tPRINTLN;

"const"                      return tCONST;
"void"                       return tTYPE_VOID;
"int"                        return tTYPE_INT;
"real"                       return tTYPE_REAL;
"string"                     return tTYPE_STRING;
"public"                     return tPUBLIC;
"use"                        return tUSE;
"next"                       return tNEXT;
"stop"                       return tSTOP;
"return"                     return tRETURN;

[_A-Za-z][A-Za-z0-9_]*       yylval.s = new std::string(yytext); return tIDENTIFIER;

0x0*[A-Fa-f1-9][A-Fa-f0-9]*  { if (!parse_int(yytext, 16, &yylval.i)) yyerror("integer out of range"); return tINTEGER; }
0b0*1[01]*                   { if (!parse_int(yytext + 2, 2, &yylval.i)) yyerror("integer out of range"); return tINTEGER; }
0+[1-7][0-7]*                { if (!parse_int(yytext, 8, &yylval.i)) yyerror("integer out of range"); return tINTEGER; }
0|[1-9][0-9]*                { if (!parse_int(yytext, 10, &yylval.i)) yyerror("integer out of range"); return tINTEGER; }

[0-9]*"."[0-9]+([E][+-]?[0-9]+)?  yylval.d = strtod(yytext, nullptr); return tREAL;
[0-9]+"."[0-9]*              yylval.d = strtod(yytext, nullptr); return tREAL;
[0-9]+y[0-9]+                yylval.d = engineering(yytext, 1E-24); return tREAL;
[0-9]+z[0-9]+                yylval.d = engineering(yytext, 1E-21); return tREAL;
[0-9]+a[0-9]+                yylval.d = engineering(yytext, 1E-18); return tREAL;
[0-9]+f[0-9]+                yylval.d = engineering(yytext, 1E-15); return tREAL;
[0-9]+p[0-9]+                yylval.d = engineering(yytext, 1E-12); return tREAL;
[0-9]+n[0-9]+                yylval.d = engineering(yytext, 1E-9); return tREAL;
[0-9]+u[0-9]+                yylval.d = engineering(yytext, 1E-6); return tREAL;
[0-9]+m[0-9]+                yylval.d = engineering(yytext, 1E-3); return tREAL;
[0-9]+k[0-9]+                yylval.d = engineering(yytext, 1E3); return tREAL;
[0-9]+M[0-9]+                yylval.d = engineering(yytext, 1E6); return tREAL;
[0-9]+G[0-9]+                yylval.d = engineering(yytext, 1E9); return tREAL;
[0-9]+T[0-9]+                yylval.d = engineering(yytext, 1E12); return tREAL;
[0-9]+P[0-9]+                yylval.d = engineering(yytext, 1E15); return tREAL;
[0-9]+E[0-9]+                yylval.d = engineering(yytext, 1E18); return tREAL;
[0-9]+Z[0-9]+                yylval.d = engineering(yytext, 1E21); return tREAL;
[0-9]+Y[0-9]+                yylval.d = engineering(yytext, 1E24); return tREAL;

[-@()<>=^+*#~?/.%;{}!:,\[\]] return *yytext;

\'                           yy_push_state(X_CHAR); yylval.s = new std::string();
<X_CHAR>\'                   yy_pop_state(); return tCHAR;
<X_CHAR>\\                   yy_push_state(X_CHAR_ESCAPE);
<X_CHAR>.                    *yylval.s += yytext;
<X_CHAR>\n                   yyerror("newline in character");

<X_CHAR_ESCAPE>n             yy_pop_state(); *yylval.s += '\n';
<X_CHAR_ESCAPE>r             yy_pop_state(); *yylval.s += '\r';
<X_CHAR_ESCAPE>t             yy_pop_state(); *yylval.s += '\t';
<X_CHAR_ESCAPE>[\'\\]        yy_pop_state(); *yylval.s += yytext;
<X_CHAR_ESCAPE>[0-9a-fA-F][0-9a-fA-F]?  yy_pop_state(); *yylval.s += (char)strtol(yytext, nullptr, 16);
<X_CHAR_ESCAPE>.|\n          yyerror("invalid escape sequence");

\"                           yy_push_state(X_STRING); yylval.s = new std::string();
<X_STRING>\"                 yy_pop_state(); return tTEXT;
<X_STRING>\\                 yy_push_state(X_STRING_ESCAPE);
<X_STRING>\0                 yyerror("NUL character in string");
<X_STRING>\n                 yyerror("newline in string");
<X_STRING>.                  *yylval.s += yytext;

<X_STRING_ESCAPE>n           yy_pop_state(); *yylval.s += '\n';
<X_STRING_ESCAPE>r           yy_pop_state(); *yylval.s += '\r';
<X_STRING_ESCAPE>t           yy_pop_state(); *yylval.s += '\t';
<X_STRING_ESCAPE>[\"\\]      yy_pop_state(); *yylval.s += yytext;
<X_STRING_ESCAPE>[0-9a-fA-F][0-9a-fA-F]?  yy_pop_state(); *yylval.s += (char)strtol(yytext, nullptr, 16);
<X_STRING_ESCAPE>.|\n        yyerror("invalid escape sequence");

[ \t\r\n]+                   ; /* ignore whitespace */

.                            yyerror("unknown character");

%%
