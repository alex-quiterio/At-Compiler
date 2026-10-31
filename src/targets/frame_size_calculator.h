#ifndef __AT_TARGETS_FRAME_SIZE_CALCULATOR_H__
#define __AT_TARGETS_FRAME_SIZE_CALCULATOR_H__

#include "targets/basic_ast_visitor.h"

namespace at {

  //! Adds up the bytes a function body needs for its local variables:
  //! every declaration in its blocks and for-loop headers. Nested function
  //! bodies are skipped (they get frames of their own), and the return
  //! slot is added by the caller. Locals never share space, so the code
  //! generator can hand out offsets in the same order.
  class frame_size_calculator: public basic_ast_visitor {
    size_t _localsize = 0;

  public:
    frame_size_calculator(std::shared_ptr<cdk::compiler> compiler) :
        basic_ast_visitor(compiler) {
    }

    size_t localsize() const { return _localsize; }

  public:
    // do not edit these lines
#define __IN_VISITOR_HEADER__
#include ".auto/visitor_decls.h"       // automatically generated
#undef __IN_VISITOR_HEADER__
    // do not edit these lines: end
  };

} // at

#endif
