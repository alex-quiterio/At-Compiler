#include <string>
#include <cdk/types/types.h>
#include "targets/postfix_writer.h"
#include "targets/type_checker.h"
#include "targets/frame_size_calculator.h"
#include ".auto/all_nodes.h"  // automatically generated
#include "at_parser.tab.h"    // tPUBLIC, tUSE

// Labels the RTS defines, plus the entry point. A function or variable
// defined in an At file under one of these names is renamed.
static const std::set<std::string> reserved = {
  "_main", "_start", "_exit", "end", "argc", "argv", "envp", "errno",
  "printi", "printd", "prints", "println", "readi", "readd",
};

static const char *const power_label = "_atrt_power";

static bool is_double(std::shared_ptr<cdk::basic_type> t) { return t->name() == cdk::TYPE_DOUBLE; }
static bool is_void(std::shared_ptr<cdk::basic_type> t) { return t->name() == cdk::TYPE_VOID; }

//! size of what a pointer type points to (4 for anything but real)
static int element_size(std::shared_ptr<cdk::basic_type> pointer) {
  auto referenced = cdk::reference_type::cast(pointer)->referenced();
  return referenced->size() > 0 ? referenced->size() : 1;
}

//---------------------------------------------------------------------------
//     HELPERS
//---------------------------------------------------------------------------

//! Assembly name of a top-level function or variable. The function "at"
//! is the program's entry point, "_main". A name defined here that clashes
//! with the RTS, or that starts with "_" (as the compiler's own labels do),
//! gets an "_at_" prefix. Names that are only declared ("use", forward
//! declarations never defined) keep their name: they are defined elsewhere.
std::string at::postfix_writer::global_label(const std::string &name, bool function) {
  if (function && name == "at") return "_main";
  if (_defined_globals.count(name) && (reserved.count(name) || name[0] == '_')) return "_at_" + name;
  return name;
}

void at::postfix_writer::load(std::shared_ptr<cdk::basic_type> type) {
  if (is_double(type)) _pf.LDDOUBLE();
  else _pf.LDINT();
}

void at::postfix_writer::store(std::shared_ptr<cdk::basic_type> type) {
  if (is_double(type)) _pf.STDOUBLE();
  else _pf.STINT();
}

//! The value of `node` is on the stack: convert it for a destination of type `to`.
void at::postfix_writer::convert(cdk::expression_node *node, std::shared_ptr<cdk::basic_type> to) {
  if (is_double(to) && node->is_typed(cdk::TYPE_INT)) _pf.I2D();
}

//! Evaluate a condition to an int that is zero when false.
void at::postfix_writer::condition(cdk::expression_node *node) {
  CHECK_TYPES(node);
  node->accept(this, 0);
  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DOUBLE(0);
    _pf.DCMP();
  }
}

//! Evaluate an expression for its side effects only.
void at::postfix_writer::discard(cdk::expression_node *node) {
  CHECK_TYPES(node);
  node->accept(this, 0);
  if (!is_void(node->type())) _pf.TRASH(node->type()->size());
}

//! Push the frame pointer (ebp) of the function at nesting `depth`, from
//! inside the current function, following static links.
void at::postfix_writer::frame_pointer(int depth) {
  int current = _symtab.depth();
  if (depth == current) {
    _pf.LOCAL(0);
    return;
  }
  _pf.LOCAL(8);   // this function's static link
  _pf.LDINT();
  for (int d = current - 1; d > depth; d--) {
    _pf.INT(8);
    _pf.ADD();
    _pf.LDINT();
  }
}

//! Push the address of a variable.
void at::postfix_writer::address(std::shared_ptr<at::symbol> symbol) {
  if (symbol->global()) {
    _pf.ADDR(symbol->label());
  } else if (symbol->depth() == _symtab.depth()) {
    _pf.LOCAL(symbol->offset());
  } else {
    frame_pointer(symbol->depth());
    _pf.INT(symbol->offset());
    _pf.ADD();
  }
}

//---------------------------------------------------------------------------
//     NODES THE CODE GENERATOR NEVER SEES
//---------------------------------------------------------------------------

void at::postfix_writer::do_nil_node(cdk::nil_node *const node, int lvl) {}
void at::postfix_writer::do_data_node(cdk::data_node *const node, int lvl) {}
void at::postfix_writer::do_balanced3_node(cdk::balanced3_node *const node, int lvl) {}
void at::postfix_writer::do_posit3_node(cdk::posit3_node *const node, int lvl) {}
void at::postfix_writer::do_takum3_node(cdk::takum3_node *const node, int lvl) {}

void at::postfix_writer::do_sequence_node(cdk::sequence_node *const node, int lvl) {
  for (auto n : node->nodes())
    if (n) n->accept(this, lvl);
}

//---------------------------------------------------------------------------
//     LITERALS
//---------------------------------------------------------------------------

void at::postfix_writer::do_integer_node(cdk::integer_node *const node, int lvl) {
  _pf.INT(node->value());
}

void at::postfix_writer::do_double_node(cdk::double_node *const node, int lvl) {
  _pf.DOUBLE(node->value());
}

void at::postfix_writer::do_string_node(cdk::string_node *const node, int lvl) {
  std::string label = newlbl();
  _pf.RODATA();
  _pf.ALIGN();
  _pf.LABEL(label);
  _pf.SSTRING(node->value());
  _pf.TEXT();
  _pf.ADDR(label);
}

//---------------------------------------------------------------------------
//     ARITHMETIC AND LOGIC
//---------------------------------------------------------------------------

void at::postfix_writer::do_unary_minus_node(cdk::unary_minus_node *const node, int lvl) {
  node->argument()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE)) _pf.DNEG();
  else _pf.NEG();
}

void at::postfix_writer::do_unary_plus_node(cdk::unary_plus_node *const node, int lvl) {
  node->argument()->accept(this, lvl);
}

void at::postfix_writer::do_not_node(cdk::not_node *const node, int lvl) {
  node->argument()->accept(this, lvl);
  _pf.INT(0);
  _pf.EQ();
}

//! int or real operands; ints are converted when the result is real.
void at::postfix_writer::arithmetic(cdk::binary_operation_node *node, void (cdk::basic_postfix_emitter::*iop)(),
                                    void (cdk::basic_postfix_emitter::*dop)()) {
  node->left()->accept(this, 0);
  convert(node->left(), node->type());
  node->right()->accept(this, 0);
  convert(node->right(), node->type());
  (_pf.*(is_double(node->type()) ? dop : iop))();
}

void at::postfix_writer::do_add_node(cdk::add_node *const node, int lvl) {
  if (node->left()->is_typed(cdk::TYPE_POINTER)) {         // p + n
    node->left()->accept(this, lvl);
    node->right()->accept(this, lvl);
    _pf.INT(element_size(node->type()));
    _pf.MUL();
    _pf.ADD();
  } else if (node->right()->is_typed(cdk::TYPE_POINTER)) { // n + p
    node->left()->accept(this, lvl);
    _pf.INT(element_size(node->type()));
    _pf.MUL();
    node->right()->accept(this, lvl);
    _pf.ADD();
  } else {
    arithmetic(node, &cdk::basic_postfix_emitter::ADD, &cdk::basic_postfix_emitter::DADD);
  }
}

void at::postfix_writer::do_sub_node(cdk::sub_node *const node, int lvl) {
  if (node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_POINTER)) {
    node->left()->accept(this, lvl);   // distance in elements
    node->right()->accept(this, lvl);
    _pf.SUB();
    _pf.INT(element_size(node->left()->type()));
    _pf.DIV();
  } else if (node->left()->is_typed(cdk::TYPE_POINTER)) {
    node->left()->accept(this, lvl);
    node->right()->accept(this, lvl);
    _pf.INT(element_size(node->type()));
    _pf.MUL();
    _pf.SUB();
  } else {
    arithmetic(node, &cdk::basic_postfix_emitter::SUB, &cdk::basic_postfix_emitter::DSUB);
  }
}

void at::postfix_writer::do_mul_node(cdk::mul_node *const node, int lvl) {
  arithmetic(node, &cdk::basic_postfix_emitter::MUL, &cdk::basic_postfix_emitter::DMUL);
}

void at::postfix_writer::do_div_node(cdk::div_node *const node, int lvl) {
  arithmetic(node, &cdk::basic_postfix_emitter::DIV, &cdk::basic_postfix_emitter::DDIV);
}

void at::postfix_writer::do_mod_node(cdk::mod_node *const node, int lvl) {
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.MOD();
}

//! Integer power, done by a small routine emitted once per file.
void at::postfix_writer::do_power_node(at::power_node *const node, int lvl) {
  node->left()->accept(this, lvl);   // base
  node->right()->accept(this, lvl);  // exponent
  _pf.CALL(power_label);
  _pf.TRASH(8);
  _pf.LDFVAL32I();
  _uses_power = true;
}

//! base ^ exponent for ints: the exponent is at ebp+8, the base at ebp+12.
//! A negative exponent gives 1.
void at::postfix_writer::emit_power() {
  std::string loop = newlbl(), done = newlbl();
  _pf.TEXT();
  _pf.ALIGN();
  _pf.LABEL(power_label);
  _pf.ENTER(4);
  _pf.INT(1);
  _pf.LOCAL(-4);
  _pf.STINT();
  _pf.LABEL(loop);
  _pf.LOCAL(8);
  _pf.LDINT();
  _pf.INT(0);
  _pf.JLE(done);
  _pf.LOCAL(-4);   // result *= base
  _pf.LDINT();
  _pf.LOCAL(12);
  _pf.LDINT();
  _pf.MUL();
  _pf.LOCAL(-4);
  _pf.STINT();
  _pf.LOCAL(8);    // exponent--
  _pf.LDINT();
  _pf.INT(1);
  _pf.SUB();
  _pf.LOCAL(8);
  _pf.STINT();
  _pf.JMP(loop);
  _pf.LABEL(done);
  _pf.LOCAL(-4);
  _pf.LDINT();
  _pf.STFVAL32I();
  _pf.LEAVE();
  _pf.RET();
}

//! Reals are compared with DCMP, which leaves the sign of left - right.
void at::postfix_writer::comparison(cdk::binary_operation_node *node, void (cdk::basic_postfix_emitter::*op)()) {
  bool reals = node->left()->is_typed(cdk::TYPE_DOUBLE) || node->right()->is_typed(cdk::TYPE_DOUBLE);
  auto real = cdk::primitive_type::create(8, cdk::TYPE_DOUBLE);
  node->left()->accept(this, 0);
  if (reals) convert(node->left(), real);
  node->right()->accept(this, 0);
  if (reals) {
    convert(node->right(), real);
    _pf.DCMP();
    _pf.INT(0);
  }
  (_pf.*op)();
}

void at::postfix_writer::do_lt_node(cdk::lt_node *const node, int lvl) { comparison(node, &cdk::basic_postfix_emitter::LT); }
void at::postfix_writer::do_le_node(cdk::le_node *const node, int lvl) { comparison(node, &cdk::basic_postfix_emitter::LE); }
void at::postfix_writer::do_ge_node(cdk::ge_node *const node, int lvl) { comparison(node, &cdk::basic_postfix_emitter::GE); }
void at::postfix_writer::do_gt_node(cdk::gt_node *const node, int lvl) { comparison(node, &cdk::basic_postfix_emitter::GT); }
void at::postfix_writer::do_ne_node(cdk::ne_node *const node, int lvl) { comparison(node, &cdk::basic_postfix_emitter::NE); }
void at::postfix_writer::do_eq_node(cdk::eq_node *const node, int lvl) { comparison(node, &cdk::basic_postfix_emitter::EQ); }

//! Short-circuit "&&" and "||", giving 0 or 1.
void at::postfix_writer::logical(cdk::binary_operation_node *node, bool is_and) {
  std::string end = newlbl();
  node->left()->accept(this, 0);
  _pf.INT(0);
  _pf.NE();
  _pf.DUP32();
  if (is_and) _pf.JZ(end);
  else _pf.JNZ(end);
  _pf.TRASH(4);
  node->right()->accept(this, 0);
  _pf.INT(0);
  _pf.NE();
  _pf.LABEL(end);
}

void at::postfix_writer::do_and_node(cdk::and_node *const node, int lvl) { logical(node, true); }
void at::postfix_writer::do_or_node(cdk::or_node *const node, int lvl) { logical(node, false); }

//---------------------------------------------------------------------------
//     VARIABLES AND MEMORY
//---------------------------------------------------------------------------

void at::postfix_writer::do_variable_node(cdk::variable_node *const node, int lvl) {
  address(_symtab.find(node->name()));
}

void at::postfix_writer::do_nested_variable_node(at::nested_variable_node *const node, int lvl) {
  address(_symtab.find(node->name(), _symtab.depth() - node->depth()));
}

void at::postfix_writer::do_rvalue_node(cdk::rvalue_node *const node, int lvl) {
  node->lvalue()->accept(this, lvl);
  load(node->type());
}

void at::postfix_writer::do_assignment_node(cdk::assignment_node *const node, int lvl) {
  node->rvalue()->accept(this, lvl);
  convert(node->rvalue(), node->type());
  if (is_double(node->type())) _pf.DUP64();
  else _pf.DUP32();
  node->lvalue()->accept(this, lvl);
  store(node->type());
}

void at::postfix_writer::do_index_node(at::index_node *const node, int lvl) {
  node->base()->accept(this, lvl);
  node->index()->accept(this, lvl);
  _pf.INT(element_size(node->base()->type()));
  _pf.MUL();
  _pf.ADD();
}

void at::postfix_writer::do_address_of_node(at::address_of_node *const node, int lvl) {
  node->lvalue()->accept(this, lvl);
}

//! "#n" reserves n elements on the stack and yields their address.
void at::postfix_writer::do_alloc_node(at::alloc_node *const node, int lvl) {
  node->argument()->accept(this, lvl);
  _pf.INT(element_size(node->type()));
  _pf.MUL();
  _pf.ALLOC();
  _pf.SP();
}

void at::postfix_writer::do_read_node(at::read_node *const node, int lvl) {
  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.CALL("readd");
    _pf.LDFVAL64F();
  } else {
    _pf.CALL("readi");
    _pf.LDFVAL32I();
  }
}

//---------------------------------------------------------------------------
//     INSTRUCTIONS
//---------------------------------------------------------------------------

void at::postfix_writer::do_evaluation_node(at::evaluation_node *const node, int lvl) {
  discard(node->argument());
}

void at::postfix_writer::do_print_node(at::print_node *const node, int lvl) {
  auto arg = node->argument();
  CHECK_TYPES(arg);
  arg->accept(this, lvl);
  if (arg->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.CALL("printd");
    _pf.TRASH(8);
  } else if (arg->is_typed(cdk::TYPE_STRING)) {
    _pf.CALL("prints");
    _pf.TRASH(4);
  } else if (arg->is_typed(cdk::TYPE_INT) || arg->is_typed(cdk::TYPE_POINTER)) {
    _pf.CALL("printi");
    _pf.TRASH(4);
  } else {
    error(node->lineno(), "cannot print a value of type " + arg->type()->to_string());
    return;
  }
  if (node->newline()) _pf.CALL("println");
}

void at::postfix_writer::do_block_node(at::block_node *const node, int lvl) {
  _symtab.push();
  node->declarations()->accept(this, lvl + 2);
  node->instructions()->accept(this, lvl + 2);
  _symtab.pop();
}

void at::postfix_writer::do_if_node(at::if_node *const node, int lvl) {
  std::string otherwise = newlbl();
  condition(node->condition());
  _pf.JZ(otherwise);
  node->block()->accept(this, lvl + 2);
  if (node->elseblock()) {
    std::string end = newlbl();
    _pf.JMP(end);
    _pf.LABEL(otherwise);
    node->elseblock()->accept(this, lvl + 2);
    _pf.LABEL(end);
  } else {
    _pf.LABEL(otherwise);
  }
}

void at::postfix_writer::do_if_else_node(at::if_else_node *const node, int lvl) {
  std::string otherwise = newlbl(), end = newlbl();
  condition(node->condition());
  _pf.JZ(otherwise);
  node->thenblock()->accept(this, lvl + 2);
  _pf.JMP(end);
  _pf.LABEL(otherwise);
  node->elseblock()->accept(this, lvl + 2);
  _pf.LABEL(end);
}

void at::postfix_writer::do_for_node(at::for_node *const node, int lvl) {
  std::string test = newlbl(), step = newlbl(), end = newlbl();
  _symtab.push();
  if (node->declarations()) node->declarations()->accept(this, lvl + 2);
  if (node->init()) discard(node->init());
  _pf.LABEL(test);
  if (node->condition()) {
    condition(node->condition());
    _pf.JZ(end);
  }
  _next_labels.push_back(step);
  _stop_labels.push_back(end);
  node->block()->accept(this, lvl + 2);
  _next_labels.pop_back();
  _stop_labels.pop_back();
  _pf.LABEL(step);
  if (node->step()) discard(node->step());
  _pf.JMP(test);
  _pf.LABEL(end);
  _symtab.pop();
}

void at::postfix_writer::do_next_node(at::next_node *const node, int lvl) {
  if (_next_labels.empty()) return error(node->lineno(), "'next' outside a loop");
  _pf.JMP(_next_labels.back());
}

void at::postfix_writer::do_stop_node(at::stop_node *const node, int lvl) {
  if (_stop_labels.empty()) return error(node->lineno(), "'stop' outside a loop");
  _pf.JMP(_stop_labels.back());
}

void at::postfix_writer::do_return_node(at::return_node *const node, int lvl) {
  _pf.JMP(_return_label);
}

//---------------------------------------------------------------------------
//     FUNCTION BODIES
//---------------------------------------------------------------------------

//! The initial section's declarations stay visible in the whole body. A
//! "return" before the final section jumps to it; in it, to the exit.
void at::postfix_writer::do_body_node(at::body_node *const node, int lvl) {
  std::string saved_end = _sections_end_label;
  _sections_end_label = newlbl();
  _return_label = _sections_end_label;

  _symtab.push();
  if (node->initial()) {
    node->initial()->declarations()->accept(this, lvl + 2);
    node->initial()->instructions()->accept(this, lvl + 2);
  }
  node->sections()->accept(this, lvl + 2);
  _pf.LABEL(_sections_end_label);
  _return_label = _epilogue_label;
  if (node->final()) node->final()->accept(this, lvl + 2);
  _symtab.pop();

  _sections_end_label = saved_end;
}

//! "(c) {}" and "{}" run when c holds and go on to the next section;
//! "[c] {}" skips the remaining sections when it runs.
void at::postfix_writer::do_section_node(at::section_node *const node, int lvl) {
  std::string skip = newlbl();
  if (node->condition()) {
    condition(node->condition());
    _pf.JZ(skip);
  }
  node->block()->accept(this, lvl + 2);
  if (node->exclusive()) _pf.JMP(_sections_end_label);
  _pf.LABEL(skip);
}

//---------------------------------------------------------------------------
//     DECLARATIONS
//---------------------------------------------------------------------------

void at::postfix_writer::do_file_node(at::file_node *const node, int lvl) {
  // which top-level names get a definition here (see global_label)
  for (auto n : node->declarations()->nodes()) {
    if (auto f = dynamic_cast<at::function_definition_node*>(n))
      _defined_globals.insert(f->name());
    else if (auto v = dynamic_cast<at::variable_declaration_node*>(n))
      if (v->qualifier() != tUSE) _defined_globals.insert(v->name());
  }

  node->declarations()->accept(this, lvl + 2);

  if (_uses_power) emit_power();

  for (auto name : {"printi", "printd", "prints", "println", "readi", "readd"})
    _pf.EXTERN(name);
  for (auto &f : _globals)
    if (!f->defined()) _pf.EXTERN(f->label());
}

//! Variables: global, local, or (from function headers) arguments, which
//! do_function_definition_node handles itself.
void at::postfix_writer::do_variable_declaration_node(at::variable_declaration_node *const node, int lvl) {
  if (_symtab.depth() == 0) return global_variable(node);

  if (_symtab.find_local(node->name())) return error(node->lineno(), "'" + node->name() + "' redeclared");
  if (is_void(node->type())) return error(node->lineno(), "variables cannot be void");

  auto symbol = std::make_shared<at::symbol>(node->name(), node->type(), false, _symtab.depth());
  _offset -= node->type()->size();
  symbol->offset(_offset);

  if (auto init = node->initializer()) {
    CHECK_TYPES_FOR(init, node->type());
    init->accept(this, lvl);
    convert(init, node->type());
    _pf.LOCAL(symbol->offset());
    store(node->type());
  }
  _symtab.insert(symbol);
}

void at::postfix_writer::global_variable(at::variable_declaration_node *node) {
  if (_symtab.find_local(node->name())) return error(node->lineno(), "'" + node->name() + "' redeclared");
  if (is_void(node->type())) return error(node->lineno(), "variables cannot be void");

  auto symbol = std::make_shared<at::symbol>(node->name(), node->type(), false, 0);
  _symtab.insert(symbol);

  if (node->qualifier() == tUSE) {
    symbol->label(node->name());
    _pf.EXTERN(symbol->label());
    return;
  }

  symbol->label(global_label(node->name(), false));
  symbol->defined(true);
  if (node->qualifier() == tPUBLIC) _pf.GLOBAL(symbol->label(), _pf.OBJ());
  if (node->initializer()) {
    global_initializer(node, symbol->label());
  } else {
    _pf.BSS();
    _pf.ALIGN();
    _pf.LABEL(symbol->label());
    _pf.SALLOC(node->type()->size());
  }
  _pf.TEXT();
}

//! A global's initial value must be a literal, possibly negated.
void at::postfix_writer::global_initializer(at::variable_declaration_node *node, const std::string &label) {
  auto init = node->initializer();
  CHECK_TYPES_FOR(init, node->type());

  bool negative = false;
  auto value = init;
  if (auto minus = dynamic_cast<cdk::unary_minus_node*>(value)) {
    negative = true;
    value = minus->argument();
  }

  if (auto s = dynamic_cast<cdk::string_node*>(value); s && !negative) {
    std::string text = newlbl();
    _pf.RODATA();
    _pf.ALIGN();
    _pf.LABEL(text);
    _pf.SSTRING(s->value());
    _pf.DATA();
    _pf.ALIGN();
    _pf.LABEL(label);
    _pf.SADDR(text);
  } else if (auto i = dynamic_cast<cdk::integer_node*>(value)) {
    int v = negative ? -i->value() : i->value();
    _pf.DATA();
    _pf.ALIGN();
    _pf.LABEL(label);
    if (is_double(node->type())) _pf.SDOUBLE(v);
    else _pf.SINT(v);
  } else if (auto d = dynamic_cast<cdk::double_node*>(value)) {
    _pf.DATA();
    _pf.ALIGN();
    _pf.LABEL(label);
    _pf.SDOUBLE(negative ? -d->value() : d->value());
  } else {
    error(node->lineno(), "a global's initial value must be a literal");
  }
}

//! Find or create the symbol of a function (shared by its declarations and
//! its definition) and check that the headers agree.
std::shared_ptr<at::symbol> at::postfix_writer::declare_function(at::function_declaration_node *node) {
  std::vector<std::shared_ptr<cdk::basic_type>> args;
  std::vector<cdk::expression_node*> defaults;
  for (auto n : node->arguments()->nodes()) {
    auto arg = dynamic_cast<at::variable_declaration_node*>(n);
    args.push_back(arg->type());
    defaults.push_back(arg->initializer());
  }

  auto symbol = _symtab.find_local(node->name());
  if (symbol) {
    if (!symbol->function()) {
      error(node->lineno(), "'" + node->name() + "' redeclared as a function");
      return nullptr;
    }
    bool same = symbol->type()->to_string() == node->type()->to_string() && symbol->arguments.size() == args.size();
    for (size_t ix = 0; same && ix < args.size(); ix++)
      same = symbol->arguments[ix]->to_string() == args[ix]->to_string();
    if (!same) {
      error(node->lineno(), "conflicting declarations of '" + node->name() + "'");
      return nullptr;
    }
    for (size_t ix = 0; ix < args.size(); ix++)
      if (defaults[ix]) symbol->defaults[ix] = defaults[ix];
    return symbol;
  }

  symbol = std::make_shared<at::symbol>(node->name(), node->type(), true, _symtab.depth());
  symbol->arguments = args;
  symbol->defaults = defaults;
  if (symbol->global()) {
    symbol->label(global_label(node->name(), true));
    _globals.push_back(symbol);
  } else {
    symbol->label(newlbl());
  }
  _symtab.insert(symbol);
  return symbol;
}

void at::postfix_writer::do_function_declaration_node(at::function_declaration_node *const node, int lvl) {
  auto symbol = declare_function(node);
  if (symbol && !symbol->global() && node->qualifier() == tUSE)
    error(node->lineno(), "'use' is only allowed at the top level");
}

void at::postfix_writer::do_function_definition_node(at::function_definition_node *const node, int lvl) {
  auto symbol = declare_function(node);
  if (!symbol) return;
  if (symbol->defined()) return error(node->lineno(), "function '" + node->name() + "' redefined");
  symbol->defined(true);

  // a nested function is emitted where it stands, jumped over
  std::string after;
  if (!symbol->global()) {
    after = newlbl();
    _pf.JMP(after);
  }

  _pf.TEXT();
  _pf.ALIGN();
  if (symbol->global() && (node->qualifier() == tPUBLIC || node->name() == "at"))
    _pf.GLOBAL(symbol->label(), _pf.FUNC());
  _pf.LABEL(symbol->label());

  frame_size_calculator frame(_compiler);
  node->body()->accept(&frame, lvl);
  auto type = node->type();
  _pf.ENTER(frame.localsize() + type->size());

  // this function's state, saving the enclosing one's
  int saved_offset = _offset;
  std::string saved_return = _return_label, saved_epilogue = _epilogue_label;
  auto saved_next = std::move(_next_labels), saved_stop = std::move(_stop_labels);
  _next_labels.clear();
  _stop_labels.clear();
  _offset = 0;
  _epilogue_label = newlbl();
  _return_label = _epilogue_label;

  _symtab.enter_function(symbol);
  _symtab.push();

  int offset = symbol->global() ? 8 : 12;  // past the static link, if any
  for (auto n : node->arguments()->nodes()) {
    auto arg = dynamic_cast<at::variable_declaration_node*>(n);
    auto s = std::make_shared<at::symbol>(arg->name(), arg->type(), false, _symtab.depth());
    s->offset(offset);
    offset += arg->type()->size();
    if (!_symtab.insert(s)) error(arg->lineno(), "argument '" + arg->name() + "' repeated");
  }

  std::shared_ptr<at::symbol> result;
  if (!is_void(type)) {
    result = std::make_shared<at::symbol>("@", type, false, _symtab.depth());
    _offset -= type->size();
    result->offset(_offset);
    _symtab.insert(result);
    if (auto init = node->default_return()) {
      CHECK_TYPES_FOR(init, type);
      init->accept(this, lvl);
      convert(init, type);
    } else {
      _pf.INT(0);
      if (is_double(type)) _pf.I2D();
    }
    _pf.LOCAL(result->offset());
    store(type);
  }

  node->body()->accept(this, lvl + 2);

  _pf.LABEL(_epilogue_label);
  if (result) {
    _pf.LOCAL(result->offset());
    load(type);
    if (is_double(type)) _pf.STFVAL64F();
    else _pf.STFVAL32I();
  } else if (symbol->global() && node->name() == "at") {
    _pf.INT(0);  // a void entry point exits with status 0
    _pf.STFVAL32I();
  }
  _pf.LEAVE();
  _pf.RET();

  _symtab.pop();
  _symtab.leave_function();
  _offset = saved_offset;
  _return_label = saved_return;
  _epilogue_label = saved_epilogue;
  _next_labels = std::move(saved_next);
  _stop_labels = std::move(saved_stop);

  if (!symbol->global()) _pf.LABEL(after);
}

//---------------------------------------------------------------------------
//     FUNCTION CALLS
//---------------------------------------------------------------------------

//! Arguments are pushed last first; missing trailing ones take their
//! defaults. A nested function also gets its static link, pushed last.
void at::postfix_writer::do_function_call_node(at::function_call_node *const node, int lvl) {
  CHECK_TYPES(node);
  auto f = node->name() == "@" ? _symtab.function() : _symtab.find(node->name());

  int bytes = 0;
  size_t given = node->arguments()->size();
  for (size_t ix = f->arguments.size(); ix-- > 0;) {
    auto type = f->arguments[ix];
    cdk::expression_node *arg;
    if (ix < given) {
      arg = dynamic_cast<cdk::expression_node*>(node->arguments()->node(ix));
    } else {
      arg = f->defaults[ix];
      CHECK_TYPES_FOR(arg, type);
    }
    arg->accept(this, lvl);
    convert(arg, type);
    bytes += type->size();
  }
  if (!f->global()) {
    frame_pointer(f->depth());
    bytes += 4;
  }

  _pf.CALL(f->label());
  if (bytes) _pf.TRASH(bytes);
  if (is_double(f->type())) _pf.LDFVAL64F();
  else if (!is_void(f->type())) _pf.LDFVAL32I();
}
