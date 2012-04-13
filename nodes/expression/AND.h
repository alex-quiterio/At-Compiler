// $Id: AND.h,v 1.7 2011-04-14 22:13:43 ist164714 Exp $ -*- c++ -*-
/*
 * $Log: AND.h,v $
 * Revision 1.7  2011-04-14 22:13:43  ist164714
 * *** empty log message ***
 *
 * Revision 1.1  2011-03-27 16:08:18  ist164714
 * *** empty log message ***
 *
 * Revision 1.1  2011-03-18 01:25:11  ist164714
 * *** empty log message ***
 *
 * Revision 1.6  2011/03/06 15:30:32  david
 * Major code cleanup (but no extra features or bugs added).
 * Compilation guards are now set to CDK6.
 *
 * Revision 1.5  2010/03/22 14:55:04  david
 * Expression types are now in the semantics directory but are not part of
 * the semantics package. This is because we defined them to be
 * drop-in-replaceable. That is, if the compiler provides, in its semantics
 * directory, an ExpressionType.h file (this file must contain a class called
 * ExpressionType), then that class will be used instead of the one
 * provided by the CDK.
 *
 * Revision 1.4  2009/03/15 18:55:35  david
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
 * Revision 1.5  2007/03/17 18:25:08  david
 * Major clean up in conditional compilation guards. Fixed a few bad #includes.
 *
 * Revision 1.3  2006/01/02 21:01:20  david
 * Compiler development kit with working visitors.
 *
 * Revision 1.1  2005/12/30 00:29:48  david
 * Nodes for various general uses.
 */
#ifndef __AT_NODE_EXPRESSION_AND_H__
#define __AT_NODE_EXPRESSION_AND_H__

#include <cdk/nodes/expressions/BinaryExpression.h>
#include "SemanticProcessor.h"

namespace at {
  namespace node {
    namespace expression {

      /**
       * Class for describing the Logical AND ('||') operator
       */
      class AND: public cdk::node::expression::BinaryExpression {
      public:
        /**
         * @param lineno source code line number for this nodes
         * @param left first operand
         * @param right second operand
         */
        inline AND(int lineno, cdk::node::expression::Expression *left, cdk::node::expression::Expression *right) :
          cdk::node::expression::BinaryExpression(lineno, left, right) {
        }

      public:

        /**
         * @return the name of the node's class
         */
        const char *name() const {
          return "at::node::expression::AND";
        }

        /**
         * @param sp semantic processor visitor
         * @param level syntactic tree level
         */
        virtual void accept(SemanticProcessor *sp, int level) {
          sp->processAND(this, level);
        }

      };

    } // expression
  } // node
} // at

#endif
