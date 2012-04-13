#ifndef __AT_PRINT_NODE_H__
#define __AT_PRINT_NODE_H__

#include <cdk/nodes/Node.h>
#include <cdk/nodes/expressions/Expression.h>

namespace at {
  namespace node {
    class PrintNode: public cdk::node::Node {
      cdk::node::expression::Expression *_argument;
      bool _newline;
      
    public:
      inline PrintNode(int lineno, cdk::node::expression::Expression *argument, bool newline) :
        cdk::node::Node(lineno), _argument(argument), _newline(newline) {
      }

    public:
    
      inline cdk::node::expression::Expression *argument() 
      {
        return _argument;
      }
      inline bool newline()
      {
        return _newline;
      }

    public:
      const char *name() {
        return "at::node::PrintNode";
      }

      void accept(SemanticProcessor *sp, int level) {
        sp->processPrintNode(this, level);
      }

    };

  } // namespace node
} // namespace at

#endif
