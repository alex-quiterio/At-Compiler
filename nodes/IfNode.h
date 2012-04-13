#ifndef __AT_IF_NODE_H__
#define __AT_IF_NODE_H__

#include <cdk/nodes/Node.h>
#include <cdk/nodes/expressions/Expression.h>

namespace at {
  namespace node {

    class IfNode: public cdk::node::Node {
      cdk::node::expression::Expression *_condition;
      cdk::node::Node *_block;

    public:
      inline IfNode(int lineno, cdk::node::expression::Expression *condition,
                    cdk::node::Node *block) :
        cdk::node::Node(lineno), _condition(condition), _block(block) {
      }

    public:
      inline cdk::node::expression::Expression *condition() {
        return _condition;
      }
      inline cdk::node::Node *block() {
        return _block;
      }

    public:
      const char *name() {
        return "IfNode";
      }

      void accept(SemanticProcessor *sp, int level) {
        sp->processIfNode(this, level);
      }

    }; // class

  } // namespace node
} // namespace at

#endif
