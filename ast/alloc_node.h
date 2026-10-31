#ifndef __AT_AST_ALLOC_NODE_H__
#define __AT_AST_ALLOC_NODE_H__

#include <cdk/ast/unary_operation_node.h>

namespace at {

  //! "#n": reserve n elements on the stack. The element type comes from
  //! the pointer the result is assigned to.
  class alloc_node : public cdk::unary_operation_node {
  public:
    alloc_node(int lineno, cdk::expression_node *argument) :
        cdk::unary_operation_node(lineno, argument) {
    }

    void accept(basic_ast_visitor *sp, int level) override { sp->do_alloc_node(this, level); }
  };

} // at

#endif
