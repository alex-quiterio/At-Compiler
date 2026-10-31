#ifndef __AT_AST_IF_NODE_H__
#define __AT_AST_IF_NODE_H__

#include <cdk/ast/expression_node.h>

namespace at {

  //! "[cond] # instr" and "[cond] ? instr".
  //!
  //! Consecutive "[c1] ? i1  [c2] ? i2 ..." instructions form an if /
  //! else-if ladder. The parser builds it by hanging each new conditional
  //! on the else branch of the open "?" before it, so a "?" node is
  //! chainable and its else branch is set after construction.
  class if_node : public cdk::basic_node {
    cdk::expression_node *_condition;
    cdk::basic_node *_block;
    bool _chainable;                     // written with '?' (not '#')
    cdk::basic_node *_elseblock = nullptr;

  public:
    if_node(int lineno, cdk::expression_node *condition, cdk::basic_node *block, bool chainable = false) :
        cdk::basic_node(lineno), _condition(condition), _block(block), _chainable(chainable) {
    }

    cdk::expression_node *condition() { return _condition; }
    cdk::basic_node *block() { return _block; }
    bool chainable() const { return _chainable; }
    cdk::basic_node *elseblock() { return _elseblock; }
    void elseblock(cdk::basic_node *elseblock) { _elseblock = elseblock; }

    void accept(basic_ast_visitor *sp, int level) override { sp->do_if_node(this, level); }
  };

} // at

#endif
