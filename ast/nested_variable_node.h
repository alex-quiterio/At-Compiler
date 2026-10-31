#ifndef __AT_AST_NESTED_VARIABLE_NODE_H__
#define __AT_AST_NESTED_VARIABLE_NODE_H__

#include <string>
#include <cdk/ast/lvalue_node.h>

namespace at {

  //! "@.x" is x in the enclosing function, "@.@.x" two levels up, and
  //! "@.@" the enclosing function's return value (name "@").
  class nested_variable_node : public cdk::lvalue_node {
    std::string _name;
    int _depth;

  public:
    nested_variable_node(int lineno, const std::string &name, int depth) :
        cdk::lvalue_node(lineno), _name(name), _depth(depth) {
    }

    const std::string &name() const { return _name; }
    int depth() const { return _depth; }

    void accept(basic_ast_visitor *sp, int level) override { sp->do_nested_variable_node(this, level); }
  };

} // at

#endif
