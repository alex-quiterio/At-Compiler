#ifndef __AT_NODE_EXPRESSION_POSITION_H__
#define __AT_NODE_EXPRESSION_POSITION_H__

#include <cdk/nodes/expressions/Expression.h>
#include "SemanticProcessor.h"

namespace at {
  namespace node {
    namespace expression {
    
      class Position: public cdk::node::expression::Expression 
      {
          int _qual;
          cdk::node::expression::Expression* _expr;
      public:
        inline Position(int lineno,int qual, cdk::node::expression::Expression* expr) :
          cdk::node::expression::Expression(lineno),_qual(qual), _expr(expr) {}
          
      public:
        inline cdk::node::expression::Expression* expression()
        {
            return _expr;
        }
        inline int isPointer()
        {
            return _qual;
        }
        const char *name() const 
        {
          return "at::node::expression::Position";
        }
        
        virtual void accept(SemanticProcessor *sp, int level) 
        {
          sp->processPosition(this, level);
        }
      };

    } // expression
  } // node
} // at

#endif 
