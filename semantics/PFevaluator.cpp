#include <cdk/nodes/Node.h>
#include <cdk/Compiler.h>
#include <cdk/semantics/SymbolTable.h>
#include <cdk/generators/ix86.h>

#include "semantics/PFevaluator.h"
#include "semantics/PFwriter.h"
#include "semantics/Symbol.h"

//---------------------------------------------------------------------------

/**
 * Postfix for ix86.
 * @var create and register an evaluator for ASM targets.
 */
at::semantics::PFevaluator at::semantics::PFevaluator::_thisEvaluator("asm");

//---------------------------------------------------------------------------

bool at::semantics::PFevaluator::evaluate(cdk::Compiler *compiler) {
  bool debug = true; // HACK! (should be passed as arg)

  std::ostream &os = compiler->ostream();

  // this symbol table will be used to check identifiers
  cdk::semantics::SymbolTable<Symbol> symtab;
  cdk::generator::ix86 pf(os, debug);


  // COMENTADO 
  /*pf.TEXT();
  pf.ALIGN();
  pf.GLOBL("_main", pf.FUNC());
  pf.LABEL("_main");
  pf.ENTER(0); */

  // the nasm code will be generated
  // identifiers will be checked during code generation
  //
  PFwriter pfwriter(os, symtab, pf);
  compiler->syntax()->accept(&pfwriter, 0);


  // the output stream must be the same used for the two processors
  //
  /*
  pf.INT(0);
  pf.POP();
  pf.LEAVE();
  pf.RET();*/

  // these are just a few library function imports
  //
  pf.EXTRN("readi");
  pf.EXTRN("readd");
  pf.EXTRN("printi");
  pf.EXTRN("printd");
  pf.EXTRN("prints");
  pf.EXTRN("println");

  return true;
}

//---------------------------------------------------------------------------
//     T H E    E N D
//---------------------------------------------------------------------------
