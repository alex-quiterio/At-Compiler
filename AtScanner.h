// $Id: AtScanner.h,v 1.1 2011-03-12 21:22:18 ist13500 Exp $ -*- c++ -*-
/*
 * $Log: AtScanner.h,v $
 * Revision 1.1  2011-03-12 21:22:18  ist13500
 * Results of preliminary conversion from Compact to At.
 *
 * Revision 1.10  2009/03/15 19:21:14  david
 * First public revision of the CDK4-based At compiler.
 * Revision logs were cleaned.
 *
 * Revision 1.8  2009/03/02 20:16:14  david
 * Corrected stupid recursion bug.
 *
 * Revision 1.7  2009/02/20 06:04:35  david
 * At in new version. The new CDK is independent from particular
 * tools (such as Flex and BYACC). Must adapt. This is the first version
 * and will probably change.
 *
 */
#ifndef __ATSCANNER_H__
#define __ATSCANNER_H__

#undef yyFlexLexer
#define yyFlexLexer AtScanner
#include <FlexLexer.h>

#endif
