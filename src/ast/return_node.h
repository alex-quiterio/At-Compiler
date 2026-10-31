#ifndef __AT_AST_RETURN_NODE_H__
#define __AT_AST_RETURN_NODE_H__

#include <cdk/ast/basic_node.h>

namespace at {

  //! "return": see body_node for where it jumps.
  class return_node : public cdk::basic_node {
  public:
    return_node(int lineno) :
        cdk::basic_node(lineno) {
    }

    void accept(basic_ast_visitor *sp, int level) override { sp->do_return_node(this, level); }
  };

} // at

#endif
