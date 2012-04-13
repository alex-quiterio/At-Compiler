#ifndef __AT_NODE_EXPRESSION_QM_H__
#define __AT_NODE_EXPRESSION_QM_H__

#include <cdk/nodes/expressions/UnaryExpression.h>
#include <string>
#include "SemanticProcessor.h"

namespace at {
  namespace node {
    namespace expression {

      class QuestionMark: public cdk::node::expression::UnaryExpression {
      public:
        inline QuestionMark(int lineno, cdk::node::expression::Expression *argument) :
          cdk::node::expression::UnaryExpression(lineno, argument) {}
      public:

        /**
         * @return the name of the node's class
         */
        const char *name() const {
          return "cdk::node::expression::QuestionMark";
        }

        /**
         * @param sp semantic processor visitor
         * @param level syntactic tree level
         */
        virtual void accept(SemanticProcessor *sp, int level) {
          sp->processQuestionMark(this, level);
        }

      };

    } // expression
  } // node
} // cdk

#endif
