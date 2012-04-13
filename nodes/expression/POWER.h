#ifndef __AT_NODE_EXPRESSION_POWER_H__
#define __AT_NODE_EXPRESSION_POWER_H__

#include <cdk/nodes/expressions/BinaryExpression.h>
#include "SemanticProcessor.h"

namespace at {
  namespace node {
    namespace expression {
      class POWER: public cdk::node::expression::BinaryExpression {
      public:

        inline POWER(int lineno, cdk::node::expression::Expression *left, cdk::node::expression::Expression *right) :
          cdk::node::expression::BinaryExpression(lineno, left, right) {}


      public:

        const char *name() const 
        {
          return "at::node::expression::POWER";
        }

        virtual void accept(SemanticProcessor *sp, int level) 
        {
          sp->processPOWER(this, level);
        }

      };

    } // expression
  } // node
} // at

#endif
