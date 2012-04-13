// $Id: ALLOC.h,v 1.1 2011-04-14 22:13:43 ist164714 Exp $ -*- c++ -*-
/*
 * $Log: ALLOC.h,v $
 * Revision 1.1  2011-04-14 22:13:43  ist164714
 * *** empty log message ***
 *
 * Revision 1.1  2011-03-18 01:25:11  ist164714
 * *** empty log message ***
 *
 * Nodes for various general uses.
 */
#ifndef __AT_NODE_EXPRESSION_ALLOC_H__
#define __AT_NODE_EXPRESSION_ALLOC_H__

#include <cdk/nodes/expressions/UnaryExpression.h>
#include "SemanticProcessor.h"

namespace at {
  namespace node {
    namespace expression {

      /**
       * Class for describing the Logical ALLOC ('~') operator
       */
      class ALLOC: public cdk::node::expression::UnaryExpression {
      public:
        /**
         * @param lineno source code line number for this nodes
         * @param unary argument
         */
        inline ALLOC(int lineno, cdk::node::expression::Expression *argument) :
          cdk::node::expression::UnaryExpression(lineno, argument) {
        }

      public:

        /**
         * @return the name of the node's class
         */
        const char *name() const {
          return "at::node::expression::ALLOC";
        }

        /**
         * @param sp semantic processor visitor
         * @param level syntactic tree level
         */
        virtual void accept(SemanticProcessor *sp, int level) {
          sp->processALLOC(this, level);
        }

      };

    } // expression
  } // node
} // at

#endif 
