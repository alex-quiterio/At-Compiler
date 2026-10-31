#ifndef __AT_AST_POWER_NODE_H__
#define __AT_AST_POWER_NODE_H__

#include <cdk/ast/binary_operation_node.h>

namespace at {

  //! "a ^ b", integers only.
  class power_node : public cdk::binary_operation_node {
  public:
    power_node(int lineno, cdk::expression_node *left, cdk::expression_node *right) :
        cdk::binary_operation_node(lineno, left, right) {
    }

    void accept(basic_ast_visitor *sp, int level) override { sp->do_power_node(this, level); }
  };

} // at

#endif
