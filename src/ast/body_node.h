#ifndef __AT_AST_BODY_NODE_H__
#define __AT_AST_BODY_NODE_H__

#include <cdk/ast/sequence_node.h>
#include "ast/block_node.h"

namespace at {

  //! A function body: an optional initial section "<< { }" whose variables
  //! are visible in the whole body, a list of sections, and an optional
  //! final section ">> { }". A "return" in the initial section or in a
  //! section jumps to the final section; in the final section it leaves.
  class body_node : public cdk::basic_node {
    at::block_node *_initial;      // or nullptr
    cdk::sequence_node *_sections; // of section_node, possibly empty
    at::block_node *_final;        // or nullptr

  public:
    body_node(int lineno, at::block_node *initial, cdk::sequence_node *sections, at::block_node *final) :
        cdk::basic_node(lineno), _initial(initial), _sections(sections), _final(final) {
    }

    at::block_node *initial() { return _initial; }
    cdk::sequence_node *sections() { return _sections; }
    at::block_node *final() { return _final; }

    void accept(basic_ast_visitor *sp, int level) override { sp->do_body_node(this, level); }
  };

} // at

#endif
