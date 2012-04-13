#ifndef __AT_ACCESS_POINTER_H__
#define __AT_ACCESS_POINTER_H__

#include <cdk/nodes/Node.h>
#include <string>
#include <cdk/nodes/expressions/Expression.h>
#include "nodes/expression/LeftValue.h"

namespace at {
 namespace node {
   namespace expression {

class AccessPointer: public cdk::node::expression::Expression {
	cdk::node::expression::Identifier *_id;
	cdk::node::expression::Expression *_exprs;
	cdk::node::expression::Expression *_anode;

public:
	inline AccessPointer(int lineno, cdk::node::expression::Identifier *id, cdk::node::expression::Expression *exprs) :
		cdk::node::expression::Expression(lineno), _id(id), _exprs(NULL), _anode(exprs) {}
		
	inline AccessPointer(int lineno, cdk::node::expression::Expression *exprs, cdk::node::expression::Expression *node = NULL) :
		cdk::node::expression::Expression(lineno), _id(NULL), _exprs(exprs), _anode(node) {}

public:
	inline cdk::node::expression::Identifier *Id() 
	{
		return _id;
	}
	inline cdk::node::expression::Expression *Exprs() 
	{
		return _exprs;
	}
	inline cdk::node::expression::Expression *Node() 
	{
		return _anode;
	}
public:
	const char *name() 
	{
		return "at::node::expression::AccessPointer";
	}

	void accept(SemanticProcessor *sp, int level) 
	{
		sp->processAccessPointer(this, level);
	}
   }; // class
  }  // namespace expression
 }  // namespace node
}  // namespace at

#endif   
