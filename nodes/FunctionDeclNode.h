#ifndef __AT_FUNCTION_DECL_NODE_H__
#define __AT_FUNCTION_DECL_NODE_H__

#include <cdk/nodes/Node.h>
#include <cdk/semantics/ExpressionType.h>
#include <cdk/nodes/expressions/Expression.h>

namespace at {
  namespace node {
    class FunctionDeclNode: public cdk::node::Node 
    {
      int _qualifier;
      ExpressionType *_type;
      cdk::node::expression::Identifier *_id;
      cdk::node::Node *_vars;
      cdk::node::expression::Expression *_literal;

    public:
      inline FunctionDeclNode(int lineno, int qualifier, ExpressionType *type,       cdk::node::expression::Identifier *id, cdk::node::Node *vars, cdk::node::expression::Expression *lit) :
        cdk::node::Node(lineno), _qualifier(qualifier), _type(type), _id(id), _vars(vars), _literal(lit){}

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

    public:
      const char *name() 
      {
        return "FunctionDeclNode";
      }

      void accept(SemanticProcessor *sp, int level) 
      {
        sp->processFunctionDeclNode(this, level);
      }
    }; // class
  } // namespace node
} // namespace at

#endif  
