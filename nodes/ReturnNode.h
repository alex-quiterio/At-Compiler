#ifndef __AT_RETURN_NODE_H__
#define __AT_RETURN_NODE_H__

#include <cdk/nodes/Node.h>
#include <cdk/nodes/expressions/Expression.h>

namespace at {
  namespace node {

    class ReturnNode: public cdk::node::Node {

    public:
      inline ReturnNode(int lineno) :
        cdk::node::Node(lineno){}

    public:
      const char *name() {
        return "ReturnNode";
      }

      void accept(SemanticProcessor *sp, int level) {
        sp->processReturnNode(this, level);
      }

    }; // class

  } // namespace node
} // namespace at

#endif  
