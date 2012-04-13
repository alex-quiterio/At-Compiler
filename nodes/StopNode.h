#ifndef __AT_STOP_NODE_H__
#define __AT_STOP_NODE_H__

#include <cdk/nodes/Node.h>
#include <cdk/nodes/expressions/Expression.h>

namespace at {
  namespace node {

    class StopNode: public cdk::node::Node {

    public:
      inline StopNode(int lineno) :
        cdk::node::Node(lineno){}

    public:
      const char *name() {
        return "StopNode";
      }

      void accept(SemanticProcessor *sp, int level) {
        sp->processStopNode(this, level);
      }

    }; // class

  } // namespace node
} // namespace at

#endif 
