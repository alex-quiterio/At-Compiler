#ifndef __AT_SEMANTICS_PFEVALUATOR_H__
#define __AT_SEMANTICS_PFEVALUATOR_H__

#include <iostream>
#include <cdk/semantics/Evaluator.h>

namespace cdk {
  class Compiler;
  namespace node {
    class Node;
  }
}

namespace at {
  namespace semantics {

    class PFevaluator: public cdk::semantics::Evaluator {
      static PFevaluator _thisEvaluator;

    protected:
      PFevaluator(const char *target = "asm") :
        cdk::semantics::Evaluator(target) {
      }

    public:
      bool evaluate(cdk::Compiler *compiler);

    }; // class PFevaluator

  } // namespace semantics
} // namespace at

#endif
