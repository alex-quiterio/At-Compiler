#ifndef __AT_AST_FOR_NODE_H__
#define __AT_AST_FOR_NODE_H__

#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>

namespace at {

  //! "[init; cond; step] instr". The init is either variable declarations
  //! (scoped to the loop) or an expression; any part may be missing, and a
  //! missing condition is true.
  class for_node : public cdk::basic_node {
    cdk::sequence_node *_declarations;  // or nullptr
    cdk::expression_node *_init;        // or nullptr
    cdk::expression_node *_condition;   // or nullptr
    cdk::expression_node *_step;        // or nullptr
    cdk::basic_node *_block;

  public:
    for_node(int lineno, cdk::sequence_node *declarations, cdk::expression_node *init,
             cdk::expression_node *condition, cdk::expression_node *step, cdk::basic_node *block) :
        cdk::basic_node(lineno), _declarations(declarations), _init(init), _condition(condition), _step(step),
        _block(block) {
    }

    cdk::sequence_node *declarations() { return _declarations; }
    cdk::expression_node *init() { return _init; }
    cdk::expression_node *condition() { return _condition; }
    cdk::expression_node *step() { return _step; }
    cdk::basic_node *block() { return _block; }

    void accept(basic_ast_visitor *sp, int level) override { sp->do_for_node(this, level); }
  };

} // at

#endif
