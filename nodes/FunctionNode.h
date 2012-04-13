#ifndef __AT_FUNCTIONODE_H__
#define __AT_FUNCTIONODE_H__

#include <cdk/nodes/Node.h>
#include <cdk/semantics/ExpressionType.h>
#include <cdk/nodes/expressions/Expression.h>

namespace at {
  namespace node {
    class FunctionNode: public cdk::node::Node {
    
      int _qualifier;
      ExpressionType *_type;
      cdk::node::expression::Identifier *_id;
      cdk::node::Node *_vars;
      cdk::node::expression::Expression *_literal;
      cdk::node::Node *_body;

    public:
      inline FunctionNode(int lineno, int qualifier, ExpressionType *type,       cdk::node::expression::Identifier *id, cdk::node::Node *vars, cdk::node::expression::Expression    *lit, cdk::node::Node *body) :
        cdk::node::Node(lineno), _qualifier(qualifier), _type(type), _id(id), _vars(vars), _literal(lit), _body(body) {}

    public:        
      inline int qualifier()            
      { 
        return _qualifier;
      }
      inline ExpressionType *type()     
      { 
        return _type;
      }
      inline cdk::node::expression::Identifier *id()    
      { 
        return _id;
      }
      inline cdk::node::Node *vars() 
      {
        return _vars;
      }
      inline cdk::node::expression::Expression *literal() 
      {
        return _literal;
      }
      inline cdk::node::Node *body() 
      {
        return _body;
      }

    public:
      const char *name() 
      {
        return "FunctionNode";
      }

      void accept(SemanticProcessor *sp, int level) 
      {
        sp->processFunctionNode(this, level);
      }
    }; // class
  } // namespace node
} // namespace at

#endif 
