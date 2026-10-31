#ifndef __AT_TARGETS_POSTFIX_TARGET_H__
#define __AT_TARGETS_POSTFIX_TARGET_H__

#include <cdk/targets/basic_target.h>
#include <cdk/ast/basic_node.h>
#include <cdk/emitters/postfix_ix86_emitter.h>
#include "targets/postfix_writer.h"

namespace at {

  //! The "asm" target: x86 assembly through CDK's Postfix emitter.
  class postfix_target: public cdk::basic_target {
    static postfix_target _self;

  private:
    postfix_target() :
        cdk::basic_target("asm") {
    }

  public:
    bool evaluate(std::shared_ptr<cdk::compiler> compiler) {
      at::scopes symtab;
      cdk::postfix_ix86_emitter pf(compiler);
      postfix_writer writer(compiler, symtab, pf);
      compiler->ast()->accept(&writer, 0);
      return !writer.errors();
    }
  };

} // at

#endif
