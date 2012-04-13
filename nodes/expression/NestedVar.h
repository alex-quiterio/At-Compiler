#ifndef __AT_NESTED_H__
#define __AT_NESTED_H__

#include <cdk/nodes/Node.h>
#include <string>
#include <cdk/nodes/expressions/Expression.h>
#include "nodes/expression/LeftValue.h"

namespace at {
 namespace node {
   namespace expression {

//!
//! Class for describing atrib nodes.
//!
class NestedVar: public cdk::node::expression::Expression {

	cdk::node::expression::Identifier *_id;
	size_t _nesting;

public:
	inline NestedVar(int lineno, cdk::node::expression::Identifier *id) :
		cdk::node::expression::Expression(lineno), _id(id), _nesting(1)
		{}
		
	inline NestedVar(int lineno, at::node::expression::NestedVar *var) :
		cdk::node::expression::Expression(lineno), _id(var->Id()), _nesting(var->Nesting() + 1)
		{
		     delete var;
		}
		
public:
	inline cdk::node::expression::Identifier *Id() 
	{
		return _id;
	}
	inline size_t Nesting()
	{
	    return _nesting;
	}
	
public:
	const char *name() 
	{
		return "at::node::expression::NestedVar";
	}

	void accept(SemanticProcessor *sp, int level) 
	{
		sp->processNestedVar(this, level);
	}
   }; // class
  }  // namespace expression
 }  // namespace node
}  // namespace at

#endif  
