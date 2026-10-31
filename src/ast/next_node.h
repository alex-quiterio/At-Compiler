#ifndef __AT_AST_NEXT_NODE_H__
#define __AT_AST_NEXT_NODE_H__

#include <cdk/ast/basic_node.h>

namespace at {

  //! "next": jump to the step of the innermost loop.
  class next_node : public cdk::basic_node {
  public:
    next_node(int lineno) :
        cdk::basic_node(lineno) {
    }

    void accept(basic_ast_visitor *sp, int level) override { sp->do_next_node(this, level); }
  };

} // at

#endif
