#ifndef __AT_AST_FUNCTION_DEFINITION_NODE_H__
#define __AT_AST_FUNCTION_DEFINITION_NODE_H__

#include "ast/function_declaration_node.h"
#include "ast/body_node.h"

namespace at {

  //! A function with a body. Functions can be defined inside other
  //! functions, where they can reach the enclosing function's variables.
  class function_definition_node : public function_declaration_node {
    at::body_node *_body;

  public:
    function_definition_node(int lineno, int qualifier, std::shared_ptr<cdk::basic_type> type,
                             const std::string &name, cdk::sequence_node *arguments,
                             cdk::expression_node *default_return, at::body_node *body) :
        function_declaration_node(lineno, qualifier, type, name, arguments, default_return), _body(body) {
    }

    at::body_node *body() { return _body; }

    void accept(basic_ast_visitor *sp, int level) override { sp->do_function_definition_node(this, level); }
  };

} // at

#endif
