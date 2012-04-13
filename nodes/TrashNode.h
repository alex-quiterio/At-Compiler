#ifndef __AT_TRASH_NODE_H__
#define __AT_TRASH_NODE_H__

#include <cdk/nodes/Node.h>
#include <cdk/nodes/expressions/Expression.h>

namespace at {
        namespace node {
        class TrashNode: public cdk::node::Node {
                
	cdk::node::expression::Expression *_expr;

        public:
	inline TrashNode(int lineno,cdk::node::expression::Expression *expr) :
		cdk::node::Node(lineno),_expr(expr){}
		
        public:
	inline cdk::node::expression::Expression *expression() 
	{
		return _expr;
	}
	const char *name() 
	{
		return "at::node::TrashNode";
	}
	
	void accept(SemanticProcessor *sp, int level) 
	{
		sp->processTrashNode(this, level);
	}
  }; // class
 } // namespace node
} // namespace at
#endif 
