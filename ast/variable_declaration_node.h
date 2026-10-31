#ifndef __AT_AST_VARIABLE_DECLARATION_NODE_H__
#define __AT_AST_VARIABLE_DECLARATION_NODE_H__

#include <string>
#include <cdk/ast/typed_node.h>
#include <cdk/ast/expression_node.h>

namespace at {

  //! A variable: global, local, or a function argument (whose initializer
  //! is then the argument's default value).
  class variable_declaration_node : public cdk::typed_node {
    int _qualifier;  // tPUBLIC, tUSE or 0
    bool _constant;
    std::string _name;
    cdk::expression_node *_initializer;

  public:
    variable_declaration_node(int lineno, int qualifier, bool constant, std::shared_ptr<cdk::basic_type> type,
                              const std::string &name, cdk::expression_node *initializer) :
        cdk::typed_node(lineno), _qualifier(qualifier), _constant(constant), _name(name), _initializer(initializer) {
      this->type(type);
    }

    int qualifier() const { return _qualifier; }
    bool constant() const { return _constant; }
    const std::string &name() const { return _name; }
    cdk::expression_node *initializer() { return _initializer; }

    void accept(basic_ast_visitor *sp, int level) override { sp->do_variable_declaration_node(this, level); }
  };

} // at

#endif
