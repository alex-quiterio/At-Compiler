#ifndef __AT_ATRIB_NODE_H__
#define __AT_ATRIB_NODE_H__

#include <cdk/nodes/Node.h>
#include <string>
#include <cdk/nodes/expressions/Expression.h>
#include "nodes/expression/LeftValue.h"

namespace at {
 namespace node {
   namespace expression {
class AtribNode: public cdk::node::expression::Expression {
	cdk::node::expression::Expression *_lvalue;
	cdk::node::expression::Expression *_rvalue;

public:
	inline AtribNode(int lineno, cdk::node::expression::Expression *lvalue,cdk::node::expression::Expression *rvalue) :
		cdk::node::expression::Expression(lineno), _lvalue(lvalue), _rvalue(rvalue) {}
public:
	inline cdk::node::expression::Expression *lvalue() {
		return _lvalue;
	}
	inline cdk::node::expression::Expression *rvalue() {
		return _rvalue;
	}
public:
	const char *name() {
		return "at::node::expression::AtribNode";
	}

	void accept(SemanticProcessor *sp, int level) {
		sp->processAtribNode(this, level);
	}
   }; // class
  }  // namespace expression
 }  // namespace node
}  // namespace at

#endif 
