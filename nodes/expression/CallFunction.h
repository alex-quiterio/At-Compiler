#ifndef __AT_CALLFUNCTION_H__
#define __AT_CALLFUNCTION_H__
#include <cdk/nodes/Node.h>
#include <cdk/semantics/ExpressionType.h>
#include <cdk/nodes/expressions/Expression.h>

namespace at {
  namespace node {
    namespace expression {

    class CallFunction: public cdk::node::expression::Expression {
    
      cdk::node::expression::Identifier *_id;
      cdk::node::Sequence *_vars;

    public:
      inline CallFunction(int lineno,cdk::node::expression::Identifier *id, cdk::node::Sequence *vars) :
        cdk::node::expression::Expression(lineno),_id(id), _vars(vars) {}

    public:
      inline cdk::node::expression::Identifier *id()    
      { 
        return _id;
      }
      inline cdk::node::Sequence *vars() {
        return _vars;
      }
      

    public:
      const char *name() {
        return "CallFunction";
      }

      void accept(SemanticProcessor *sp, int level) {
        sp->processCallFunction(this, level);
      }

    }; // class
   }
  } // namespace node
} // namespace at

#endif   
