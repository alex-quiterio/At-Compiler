// $Id: SemanticProcessor.cpp,v 1.9 2011-04-14 22:22:19 ist164714 Exp $
/*
 * $Log: SemanticProcessor.cpp,v $
 * Revision 1.9  2011-04-14 22:22:19  ist164714
 * *** empty log message ***
 *
 * Revision 1.1  2011-03-12 21:22:20  ist13500
 * Results of preliminary conversion from Compact to At.
 *
 * Revision 1.4  2009/03/15 19:21:15  david
 * First public revision of the CDK4-based At compiler.
 * Revision logs were cleaned.
 *
 * Revision 1.3  2009/03/02 17:40:21  david
 * Major changes: in addition to compiling with CDK4, At now has
 * its own namespace (at). All classes are defined in it or in its
 * child namespaces. Added automatic node processing: the "nodes"
 * directory is scanned and nodes/all.h is built (contains both forward
 * declarations and include directives for all nodes -- in the appropriate
 * namespaces).
 *
 */
#include "semantics/SemanticProcessor.h"
