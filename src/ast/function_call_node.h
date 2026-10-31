#ifndef __AT_AST_FUNCTION_CALL_NODE_H__
#define __AT_AST_FUNCTION_CALL_NODE_H__

#include <string>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/sequence_node.h>

namespace at {

  //! "f(args)". The name "@" calls the current function.
  class function_call_node : public cdk::expression_node {
    std::string _name;
    cdk::sequence_node *_arguments;

  public:
    function_call_node(int lineno, const std::string &name, cdk::sequence_node *arguments) :
        cdk::expression_node(lineno), _name(name), _arguments(arguments) {
    }

    const std::string &name() const { return _name; }
    cdk::sequence_node *arguments() { return _arguments; }

    void accept(basic_ast_visitor *sp, int level) override { sp->do_function_call_node(this, level); }
  };

} // at

#endif
