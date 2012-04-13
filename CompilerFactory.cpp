#include "CompilerFactory.h"
#include "syntax/FlexBYACC.h"

/**
 * This object is automatically registered by the constructor in the
 * superclass' language registry.
 */
at::CompilerFactory at::CompilerFactory::_thisFactory("at");

/**
 * Create a parser object for the At language.
 * This method is now private and is called only from the compiler creation
 * method.
 * @param name name of the input file (for debug only)
 * @return parser object pointer
 * @see createCompiler
 */
cdk::syntax::Parser *at::CompilerFactory::createParser(const char *name) {
  at::syntax::FlexBYACC *parser = new at::syntax::FlexBYACC();
  parser->scanner(new AtScanner(NULL, NULL));
  return parser;
}

/**
 * Create a compiler object for a given language.
 * @param name name of the language handled by the compiler
 * @return compiler object pointer
 */
cdk::Compiler *at::CompilerFactory::createCompiler(const char *name) {
  cdk::syntax::Parser *parser = createParser(name);
  return new cdk::Compiler(name, parser);
}
