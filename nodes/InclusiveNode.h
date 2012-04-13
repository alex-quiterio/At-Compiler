#ifndef __AT_INCLUSIVE_NODE_H__
#define __AT_INCLUSIVE_NODE_H__

#include <cdk/nodes/Node.h>
#include <cdk/nodes/expressions/Expression.h>

namespace at {
  namespace node {

    class InclusiveNode: public cdk::node::Node {
      cdk::node::expression::Expression *_condition;
      cdk::node::Node *_thenblock;
      cdk::node::Node *_elseblock;
    
    public:
      inline InclusiveNode(int lineno, cdk::node::expression::Expression *condition, cdk::node::Node *thenblock,  cdk::node::Node *elseblock) :
        cdk::node::Node(lineno), _condition(condition), _thenblock(thenblock),
            _elseblock(elseblock) {}

    public:
      inline cdk::node::expression::Expression *condition() {
        return _condition;
      }
      inline cdk::node::Node *thenblock() {
        return _thenblock;
      }
      inline cdk::node::Node *elseblock() {
        return _elseblock;
      }


    public:
      const char *name() {
        return "InclusiveNode";
      }

      void accept(SemanticProcessor *sp, int level) {
        sp->processInclusiveNode(this, level);
      }

    }; // class

  } // namespace node
} // namespace at

#endif
