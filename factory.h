#ifndef __AT_FACTORY_H__
#define __AT_FACTORY_H__

#include <memory>
#include <cdk/yy_factory.h>
#include "at_scanner.h"

namespace at {

  /**
   * Compiler factory for the At language. The single instance registers
   * itself with CDK under the name "at", which is how the driver finds it
   * (the language is taken from the executable's name).
   */
  class factory: public cdk::yy_factory<at_scanner> {
    static factory _self;

  protected:
    factory(const std::string &language = "at") :
        cdk::yy_factory<at_scanner>(language) {
    }
  };

} // at

#endif
