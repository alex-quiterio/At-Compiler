#ifndef __AT_ATCOMPILERFACTORY_H__
#define __AT_ATCOMPILERFACTORY_H__

#include <cdk/CompilerFactory.h>

class FlexLexer;
namespace cdk {
  class Compiler;
}

namespace at {

  /**
   * This class implements the compiler factory for the At compiler.
   */
  class CompilerFactory: public cdk::CompilerFactory {
    /**
     * This object is automatically registered by the constructor in the
     * superclass' language registry.
     */
    static CompilerFactory _thisFactory;

  protected:
    /**
     * @param language name of the language handled by this factory (see .cpp file)
     */
CompilerFactory  (const char *language) :
  cdk::CompilerFactory(language) {
  }

protected:
  /**
   * Create a scanner object for the At language.
   * This method is now private and is called only from the compiler creation
   * method.
   * @param name name of the input file (for debug only)
   * @return scanner object pointer
   * @see createCompiler
   */
  virtual cdk::syntax::Parser *createParser(const char *name);

public:
  /**
   * Create a compiler object for a given language.
   * @param name name of the language handled by the compiler
   * @return compiler object pointer
   */
  virtual cdk::Compiler *createCompiler(const char *name);

}; // class CompilerFactory

} // namespace at

#endif
