#ifndef __AT_DEC_NODE_H__
#define __AT_DEC_NODE_H__

#include <cdk/nodes/Node.h>
#include <string>
#include <cdk/nodes/expressions/Expression.h>
#include <cdk/semantics/ExpressionType.h>
#include "nodes/expression/LeftValue.h"

namespace at {
        namespace node {
        class DeclarationNode: public cdk::node::Node 
        {        
        int _use;
        int _const;
        int _qualifier;
      	ExpressionType *_type;
	    at::node::expression::LeftValue *_lvalue;
	    cdk::node::expression::Expression *_rvalue;

public:
	inline DeclarationNode(int lineno,int use, int cons, int qualifier, ExpressionType *type, at::node::expression::LeftValue *lvalue, cdk::node::expression::Expression *rvalue) :
		cdk::node::Node(lineno), _use(use),_const(cons), _qualifier(qualifier), _type(type),
		 _lvalue(lvalue), _rvalue(rvalue) {}

public:
	inline int is_use() 
	{
	        return _use;
	}
	inline int is_constant() 
	{
	        return _const;
	}
public:
	inline int qualifier()
	{
	        return _qualifier;
	}
	inline ExpressionType *type()
	{ 
                return _type;
    }
public:
	inline at::node::expression::LeftValue *lvalue()
	{
		return _lvalue;
	}
	inline cdk::node::expression::Expression *rvalue() 
	{
		return _rvalue;
	}
public:
	const char *name() 
	{
		return "at::node::DeclarationNode";
	}
	void accept(SemanticProcessor *sp, int level) 
	{
		sp->processDeclarationNode(this, level);
	}
  }; // class
 } // namespace node
} // namespace at
#endif
