#ifndef __AT_NODE_EXPRESSION_OR_H__
#define __AT_NODE_EXPRESSION_OR_H__

#include <cdk/nodes/expressions/BinaryExpression.h>
#include "SemanticProcessor.h"

namespace at {
  namespace node {
    namespace expression {

      /**
       * Class for describing the Logical OR ('||') operator
       */
      class OR: public cdk::node::expression::BinaryExpression {
      public:
        /**
         * @param lineno source code line number for this nodes
         * @param left first operand
         * @param right second operand
         */
        inline OR(int lineno, cdk::node::expression::Expression *left, cdk::node::expression::Expression *right) :
          cdk::node::expression::BinaryExpression(lineno, left, right) {
        }

      public:

        /**
         * @return the name of the node's class
         */
        const char *name() const {
          return "at::node::expression::OR";
        }

        /**
         * @param sp semantic processor visitor
         * @param level syntactic tree level
         */
        virtual void accept(SemanticProcessor *sp, int level) {
          sp->processOR(this, level);
        }

      };

    } // expression
  } // node
} // at

#endif
