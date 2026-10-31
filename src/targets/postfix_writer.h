#ifndef __AT_TARGETS_POSTFIX_WRITER_H__
#define __AT_TARGETS_POSTFIX_WRITER_H__

#include "targets/basic_ast_visitor.h"

#include <set>
#include <string>
#include <vector>
#include <cdk/types/basic_type.h>
#include <cdk/emitters/basic_postfix_emitter.h>

namespace at {

  //! Generates Postfix code (x86 assembly through CDK's emitter).
  //!
  //! Frames: arguments start at ebp+8, pushed last first. A function
  //! defined inside another one gets a hidden first argument, the static
  //! link (its enclosing function's ebp), and its own arguments start at
  //! ebp+12. Locals, the return slot "@" first, are below ebp.
  class postfix_writer: public basic_ast_visitor {
    at::scopes &_symtab;
    cdk::basic_postfix_emitter &_pf;
    int _lbl = 0;
    bool _errors = false;

    // whole file
    std::set<std::string> _defined_globals;            // names given a body or storage here
    std::vector<std::shared_ptr<at::symbol>> _globals; // top-level functions, for EXTERN
    bool _uses_power = false;

    // the function being generated (saved around nested functions)
    int _offset = 0;                          // last local's offset
    std::string _return_label;                // where "return" goes
    std::string _epilogue_label;              // the function's exit
    std::string _sections_end_label;          // the final section
    std::vector<std::string> _next_labels, _stop_labels;  // innermost loop last

  public:
    postfix_writer(std::shared_ptr<cdk::compiler> compiler, at::scopes &symtab, cdk::basic_postfix_emitter &pf) :
        basic_ast_visitor(compiler), _symtab(symtab), _pf(pf) {
    }

    ~postfix_writer() {
      os().flush();
    }

    bool errors() const { return _errors; }

  private:
    std::string mklbl(int lbl) { return "_L" + std::to_string(lbl); }
    std::string newlbl() { return mklbl(++_lbl); }

    void error(int lineno, const std::string &s) {
      std::cerr << "error: " << lineno << ": " << s << std::endl;
      _errors = true;
    }

    std::string global_label(const std::string &name, bool function);

    void load(std::shared_ptr<cdk::basic_type> type);
    void store(std::shared_ptr<cdk::basic_type> type);
    void convert(cdk::expression_node *node, std::shared_ptr<cdk::basic_type> to);
    void condition(cdk::expression_node *node);
    void discard(cdk::expression_node *node);
    void frame_pointer(int depth);
    void address(std::shared_ptr<at::symbol> symbol);
    void comparison(cdk::binary_operation_node *node, void (cdk::basic_postfix_emitter::*op)());
    void arithmetic(cdk::binary_operation_node *node, void (cdk::basic_postfix_emitter::*iop)(),
                    void (cdk::basic_postfix_emitter::*dop)());
    void logical(cdk::binary_operation_node *node, bool is_and);
    void global_variable(at::variable_declaration_node *node);
    void global_initializer(at::variable_declaration_node *node, const std::string &label);
    std::shared_ptr<at::symbol> declare_function(at::function_declaration_node *node);
    void emit_power();

  public:
    // do not edit these lines
#define __IN_VISITOR_HEADER__
#include ".auto/visitor_decls.h"       // automatically generated
#undef __IN_VISITOR_HEADER__
    // do not edit these lines: end
  };

} // at

#endif
