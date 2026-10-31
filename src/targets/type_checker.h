#ifndef __AT_TARGETS_TYPE_CHECKER_H__
#define __AT_TARGETS_TYPE_CHECKER_H__

#include "targets/basic_ast_visitor.h"

namespace at {

  //! Types expressions and checks them. Called by the code generator on
  //! each expression it is about to emit; problems are thrown as strings.
  class type_checker: public basic_ast_visitor {
    at::scopes &_symtab;

  public:
    type_checker(std::shared_ptr<cdk::compiler> compiler, at::scopes &symtab) :
        basic_ast_visitor(compiler), _symtab(symtab) {
    }

    //! Type `node` for a destination of type `target` (an assignment, an
    //! initializer, an argument or a return value): "@" and "#n" take their
    //! type from it, and the node must be convertible to it.
    void check_for(cdk::expression_node *node, std::shared_ptr<cdk::basic_type> target);

  protected:
    void check_numeric_binary(cdk::binary_operation_node *const node, int lvl, bool allow_double);
    void check_comparison(cdk::binary_operation_node *const node, int lvl, bool allow_pointers);
    std::shared_ptr<at::symbol> called_function(at::function_call_node *const node);

  public:
    // do not edit these lines
#define __IN_VISITOR_HEADER__
#include ".auto/visitor_decls.h"       // automatically generated
#undef __IN_VISITOR_HEADER__
    // do not edit these lines: end

  };

} // at

//---------------------------------------------------------------------------
//     HELPER MACROS FOR TYPE CHECKING
//---------------------------------------------------------------------------

//! Type-check `expr` in the code generator; on error, report and skip the node.
#define CHECK_TYPES(expr) { \
  try { \
    at::type_checker checker(_compiler, _symtab); \
    (expr)->accept(&checker, 0); \
  } \
  catch (const std::string &problem) { \
    error(node->lineno(), problem); \
    return; \
  } \
}

//! Same, for an expression bound to a destination of the given type.
#define CHECK_TYPES_FOR(expr, target) { \
  try { \
    at::type_checker checker(_compiler, _symtab); \
    checker.check_for((expr), (target)); \
  } \
  catch (const std::string &problem) { \
    error(node->lineno(), problem); \
    return; \
  } \
}

#endif
