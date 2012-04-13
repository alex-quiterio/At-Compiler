#ifndef __AT_I_ARG_NODE_H__
#define __AT_I_ARG_NODE_H__

#include <cdk/nodes/Node.h>
#include <string>
#include <cdk/nodes/expressions/Expression.h>
#include <cdk/semantics/ExpressionType.h>

namespace at {
        namespace node {
        class InputArgumentNode: public cdk::node::Node {
                
      	ExpressionType *_type;
        cdk::node::expression::Identifier *_id;
        cdk::node::expression::Expression *_lit;
public:
	inline InputArgumentNode(int lineno,ExpressionType *type,cdk::node::expression::Identifier *id,    cdk::node::expression::Expression *lit) :
		cdk::node::Node(lineno),_type(type),_id(id), _lit(lit) {}

public:
	inline ExpressionType *type()
	{ 
                return _type;
        }
public:
	inline cdk::node::expression::Identifier *id()
	{
		return _id;
	}
public:
	inline cdk::node::expression::Expression *literal()
	{
		return _lit;
	}
public:
	const char *name() 
	{
		return "at::node::InputArgumentNode";
	}
	
	void accept(SemanticProcessor *sp, int level) 
	{
		sp->processInputArgumentNode(this, level);
	}
  }; // class
 } // namespace node
} // namespace at
#endif
