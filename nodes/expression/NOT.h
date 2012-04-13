#ifndef __AT_NODE_EXPRESSION_NOT_H__
#define __AT_NODE_EXPRESSION_NOT_H__

#include <cdk/nodes/expressions/UnaryExpression.h>
#include "SemanticProcessor.h"

namespace at {
  namespace node {
    namespace expression {

      /**
       * Class for describing the Logical NOT ('~') operator
       */
      class NOT: public cdk::node::expression::UnaryExpression {
      public:
        /**
         * @param lineno source code line number for this nodes
         * @param unary argument
         */
        inline NOT(int lineno, cdk::node::expression::Expression *argument) :
          cdk::node::expression::UnaryExpression(lineno, argument) {
        }

      public:

        /**
         * @return the name of the node's class
         */
        const char *name() const {
          return "at::node::expression::NOT";
        }

        /**
         * @param sp semantic processor visitor
         * @param level syntactic tree level
         */
        virtual void accept(SemanticProcessor *sp, int level) {
          sp->processNOT(this, level);
        }

      };

    } // expression
  } // node
} // at

#endif
