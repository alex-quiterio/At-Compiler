#ifndef __AT_NEXT_NODE_H__
#define __AT_NEXT_NODE_H__

#include <cdk/nodes/Node.h>
#include <cdk/nodes/expressions/Expression.h>

namespace at {
  namespace node {

    class NextNode: public cdk::node::Node {

    public:
      inline NextNode(int lineno) :
        cdk::node::Node(lineno){}

    public:
      const char *name() {
        return "NextNode";
      }

      void accept(SemanticProcessor *sp, int level) {
        sp->processNextNode(this, level);
      }

    }; // class

  } // namespace node
} // namespace at

#endif
