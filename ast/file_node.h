#ifndef __AT_AST_FILE_NODE_H__
#define __AT_AST_FILE_NODE_H__

#include <cdk/ast/sequence_node.h>

namespace at {

  //! The whole source file: a sequence of global declarations and functions.
  class file_node : public cdk::basic_node {
    cdk::sequence_node *_declarations;

  public:
    file_node(int lineno, cdk::sequence_node *declarations) :
        cdk::basic_node(lineno), _declarations(declarations) {
    }

    cdk::sequence_node *declarations() { return _declarations; }

    void accept(basic_ast_visitor *sp, int level) override { sp->do_file_node(this, level); }
  };

} // at

#endif
