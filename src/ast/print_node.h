#ifndef __AT_AST_PRINT_NODE_H__
#define __AT_AST_PRINT_NODE_H__

#include <cdk/ast/expression_node.h>

namespace at {

  //! "expr!" prints the value; "expr!!" also prints a newline.
  class print_node : public cdk::basic_node {
    cdk::expression_node *_argument;
    bool _newline;

  public:
    print_node(int lineno, cdk::expression_node *argument, bool newline) :
        cdk::basic_node(lineno), _argument(argument), _newline(newline) {
    }

    cdk::expression_node *argument() { return _argument; }
    bool newline() const { return _newline; }

    void accept(basic_ast_visitor *sp, int level) override { sp->do_print_node(this, level); }
  };

} // at

#endif
