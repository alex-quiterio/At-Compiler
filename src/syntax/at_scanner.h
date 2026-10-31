#ifndef __AT_SCANNER_H__
#define __AT_SCANNER_H__

#undef yyFlexLexer
#define yyFlexLexer at_scanner_FlexLexer
#include <FlexLexer.h>

typedef at_scanner_FlexLexer at_scanner;

#endif
