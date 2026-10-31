#ifndef __AT_AST_FUNCTION_DECLARATION_NODE_H__
#define __AT_AST_FUNCTION_DECLARATION_NODE_H__

#include <string>
#include <cdk/ast/typed_node.h>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>

namespace at {

  //! A function header without a body: a forward declaration or, with
  //! "use", a function defined elsewhere. The node's type is the return type.
  class function_declaration_node : public cdk::typed_node {
    int _qualifier;  // tPUBLIC, tUSE or 0
    std::string _name;
    cdk::sequence_node *_arguments;           // of variable_declaration_node
    cdk::expression_node *_default_return;    // "-> literal", or nullptr

  public:
    function_declaration_node(int lineno, int qualifier, std::shared_ptr<cdk::basic_type> type,
                              const std::string &name, cdk::sequence_node *arguments,
                              cdk::expression_node *default_return) :
        cdk::typed_node(lineno), _qualifier(qualifier), _name(name), _arguments(arguments),
        _default_return(default_return) {
      this->type(type);
    }

    int qualifier() const { return _qualifier; }
    const std::string &name() const { return _name; }
    cdk::sequence_node *arguments() { return _arguments; }
    cdk::expression_node *default_return() { return _default_return; }

    void accept(basic_ast_visitor *sp, int level) override { sp->do_function_declaration_node(this, level); }
  };

} // at

#endif
