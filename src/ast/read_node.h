#ifndef __AT_AST_READ_NODE_H__
#define __AT_AST_READ_NODE_H__

#include <cdk/ast/expression_node.h>

namespace at {

  //! "@" as a value: reads an int, or a real when assigned to a real.
  class read_node : public cdk::expression_node {
  public:
    read_node(int lineno) :
        cdk::expression_node(lineno) {
    }

    void accept(basic_ast_visitor *sp, int level) override { sp->do_read_node(this, level); }
  };

} // at

#endif
