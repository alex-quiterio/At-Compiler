#ifndef __AT_READNODE_H__
#define __AT_READNODE_H__

#include <cdk/nodes/Node.h>
#include "nodes/expression/LeftValue.h"

namespace at {
  namespace node {
   namespace expression {
    class ReadNode: public cdk::node::expression::Expression {

    public:
      inline ReadNode(int lineno) :
        cdk::node::expression::Expression(lineno) {}

    public:
      const char *name() {
        return "ReadNode";
      }

      void accept(SemanticProcessor *sp, int level) {
        sp->processReadNode(this, level);
      }

    }; // class

    } // namespace expression
  } // namespace node
} // namespace at

#endif
