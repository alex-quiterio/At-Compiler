#ifndef __AT_AST_SECTION_NODE_H__
#define __AT_AST_SECTION_NODE_H__

#include <cdk/ast/expression_node.h>
#include "ast/block_node.h"

namespace at {

  //! A section of a function body. "(cond) { }" and a bare "{ }" are
  //! inclusive: the block runs when the condition holds, then the next
  //! section is tried. "[cond] { }" is exclusive: when it runs, the
  //! remaining sections are skipped. A missing condition is true.
  class section_node : public cdk::basic_node {
    bool _exclusive;
    cdk::expression_node *_condition; // or nullptr
    at::block_node *_block;

  public:
    section_node(int lineno, bool exclusive, cdk::expression_node *condition, at::block_node *block) :
        cdk::basic_node(lineno), _exclusive(exclusive), _condition(condition), _block(block) {
    }

    bool exclusive() const { return _exclusive; }
    cdk::expression_node *condition() { return _condition; }
    at::block_node *block() { return _block; }

    void accept(basic_ast_visitor *sp, int level) override { sp->do_section_node(this, level); }
  };

} // at

#endif
