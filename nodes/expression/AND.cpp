// $Id: AND.cpp,v 1.7 2011-04-14 22:12:14 ist164714 Exp $ -*- c++ -*-
/*
 * $Log: AND.cpp,v $
 * Revision 1.7  2011-04-14 22:12:14  ist164714
 * *** empty log message ***
 *
 * Revision 1.1  2011-03-27 16:08:18  ist164714
 * *** empty log message ***
 *
 * Revision 1.1  2011-03-18 01:25:11  ist164714
 * *** empty log message ***
 *
 * Revision 1.3  2011/03/06 15:30:32  david
 * Major code cleanup (but no extra features or bugs added).
 * Compilation guards are now set to CDK6.
 *
 * Revision 1.2  2009/03/15 18:55:35  david
 * This is the first public version of CDK4. The main changes are related
 * to the removal of the Symbol class from the library: each compiler
 * must supply its own representation for symbols. The symbol table is
 * now a template and is otherwise unchanged (which in some ways is
 * a real shame). All compilations guards have been updated to CDK4.
 * Major log cleanup was performed to remove old comments that might
 * have led people into despair and unhappiness (nevertheless, these
 * effects may still be achievable). Added a new constant for describing
 * structured types: this is clearly insufficient by itself, but may help in
 * porting old code.
 *
 * Revision 1.1  2008/03/01 15:54:26  david
 * First CDK3 version: changed how operators and expressions are defined.
 * They now have builtin types and do not need to be specialized for each
 * compiler: that increased complexity and was a potential source of
 * problems, since multiple inheritance was needed.
 *
 * Revision 1.1  2006/01/02 21:01:20  david
 * Compiler development kit with working visitors.
 */
#include "nodes/expression/AND.h"
