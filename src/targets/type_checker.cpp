#include <string>
#include <cdk/types/types.h>
#include "targets/type_checker.h"
#include ".auto/all_nodes.h"  // automatically generated

#define ASSERT_UNSPEC { if (node->type() != nullptr && !node->is_typed(cdk::TYPE_UNSPEC)) return; }

static auto int_type() { return cdk::primitive_type::create(4, cdk::TYPE_INT); }
static auto real_type() { return cdk::primitive_type::create(8, cdk::TYPE_DOUBLE); }

static bool untyped(cdk::typed_node *node) {
  return node->type() == nullptr || node->is_typed(cdk::TYPE_UNSPEC);
}

//---------------------------------------------------------------------------
//     DESTINATION TYPING
//---------------------------------------------------------------------------

void at::type_checker::check_for(cdk::expression_node *node, std::shared_ptr<cdk::basic_type> target) {
  if (auto read = dynamic_cast<at::read_node*>(node)) {
    if (untyped(read)) {
      if (target->name() != cdk::TYPE_INT && target->name() != cdk::TYPE_DOUBLE)
        throw std::string("can only read integers and reals");
      read->type(target);
    }
  } else if (auto alloc = dynamic_cast<at::alloc_node*>(node)) {
    if (untyped(alloc)) {
      if (target->name() != cdk::TYPE_POINTER) throw std::string("'#' must be assigned to a pointer");
      alloc->type(target);
    }
  }
  node->accept(this, 0);

  auto from = node->type()->name(), to = target->name();
  if (from == to) return;
  if (from == cdk::TYPE_INT && to == cdk::TYPE_DOUBLE) return;  // converted by the code generator
  if (from == cdk::TYPE_INT && to == cdk::TYPE_POINTER) return; // e.g. "<int> p = 0"
  throw std::string("cannot use a " + node->type()->to_string() + " as a " + target->to_string());
}

//---------------------------------------------------------------------------
//     NODES WITH NOTHING TO CHECK
//---------------------------------------------------------------------------

void at::type_checker::do_sequence_node(cdk::sequence_node *const node, int lvl) {}
void at::type_checker::do_nil_node(cdk::nil_node *const node, int lvl) {}
void at::type_checker::do_data_node(cdk::data_node *const node, int lvl) {}
void at::type_checker::do_balanced3_node(cdk::balanced3_node *const node, int lvl) {}
void at::type_checker::do_posit3_node(cdk::posit3_node *const node, int lvl) {}
void at::type_checker::do_takum3_node(cdk::takum3_node *const node, int lvl) {}
void at::type_checker::do_file_node(at::file_node *const node, int lvl) {}
void at::type_checker::do_variable_declaration_node(at::variable_declaration_node *const node, int lvl) {}
void at::type_checker::do_function_declaration_node(at::function_declaration_node *const node, int lvl) {}
void at::type_checker::do_function_definition_node(at::function_definition_node *const node, int lvl) {}
void at::type_checker::do_body_node(at::body_node *const node, int lvl) {}
void at::type_checker::do_section_node(at::section_node *const node, int lvl) {}
void at::type_checker::do_block_node(at::block_node *const node, int lvl) {}
void at::type_checker::do_evaluation_node(at::evaluation_node *const node, int lvl) {}
void at::type_checker::do_print_node(at::print_node *const node, int lvl) {}
void at::type_checker::do_next_node(at::next_node *const node, int lvl) {}
void at::type_checker::do_stop_node(at::stop_node *const node, int lvl) {}
void at::type_checker::do_return_node(at::return_node *const node, int lvl) {}
void at::type_checker::do_if_node(at::if_node *const node, int lvl) {}
void at::type_checker::do_if_else_node(at::if_else_node *const node, int lvl) {}
void at::type_checker::do_for_node(at::for_node *const node, int lvl) {}

//---------------------------------------------------------------------------
//     LITERALS
//---------------------------------------------------------------------------

void at::type_checker::do_integer_node(cdk::integer_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(int_type());
}

void at::type_checker::do_double_node(cdk::double_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(real_type());
}

void at::type_checker::do_string_node(cdk::string_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
}

//---------------------------------------------------------------------------
//     ARITHMETIC AND LOGIC
//---------------------------------------------------------------------------

void at::type_checker::do_unary_minus_node(cdk::unary_minus_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  if (!node->argument()->is_typed(cdk::TYPE_INT) && !node->argument()->is_typed(cdk::TYPE_DOUBLE))
    throw std::string("'-' needs an integer or a real");
  node->type(node->argument()->type());
}

void at::type_checker::do_unary_plus_node(cdk::unary_plus_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  if (!node->argument()->is_typed(cdk::TYPE_INT) && !node->argument()->is_typed(cdk::TYPE_DOUBLE))
    throw std::string("'+' needs an integer or a real");
  node->type(node->argument()->type());
}

void at::type_checker::do_not_node(cdk::not_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  if (!node->argument()->is_typed(cdk::TYPE_INT)) throw std::string("'~' needs an integer");
  node->type(int_type());
}

//! int op int is int; with a real on either side, the result is real.
void at::type_checker::check_numeric_binary(cdk::binary_operation_node *const node, int lvl, bool allow_double) {
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  bool li = node->left()->is_typed(cdk::TYPE_INT), ri = node->right()->is_typed(cdk::TYPE_INT);
  bool ld = node->left()->is_typed(cdk::TYPE_DOUBLE), rd = node->right()->is_typed(cdk::TYPE_DOUBLE);
  if (li && ri)
    node->type(int_type());
  else if (allow_double && (li || ld) && (ri || rd))
    node->type(real_type());
  else
    throw std::string(allow_double ? "operands must be integers or reals" : "operands must be integers");
}

void at::type_checker::do_add_node(cdk::add_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  if (node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT))
    node->type(node->left()->type());
  else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_POINTER))
    node->type(node->right()->type());
  else
    check_numeric_binary(node, lvl, true);
}

void at::type_checker::do_sub_node(cdk::sub_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  if (node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT))
    node->type(node->left()->type());
  else if (node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_POINTER))
    node->type(int_type());  // distance in elements
  else
    check_numeric_binary(node, lvl, true);
}

void at::type_checker::do_mul_node(cdk::mul_node *const node, int lvl) {
  ASSERT_UNSPEC;
  check_numeric_binary(node, lvl, true);
}
void at::type_checker::do_div_node(cdk::div_node *const node, int lvl) {
  ASSERT_UNSPEC;
  check_numeric_binary(node, lvl, true);
}
void at::type_checker::do_mod_node(cdk::mod_node *const node, int lvl) {
  ASSERT_UNSPEC;
  check_numeric_binary(node, lvl, false);
}
void at::type_checker::do_power_node(at::power_node *const node, int lvl) {
  ASSERT_UNSPEC;
  check_numeric_binary(node, lvl, false);
}
void at::type_checker::do_and_node(cdk::and_node *const node, int lvl) {
  ASSERT_UNSPEC;
  check_numeric_binary(node, lvl, false);
}
void at::type_checker::do_or_node(cdk::or_node *const node, int lvl) {
  ASSERT_UNSPEC;
  check_numeric_binary(node, lvl, false);
}

//! Comparisons are int-valued; reals are compared as reals.
void at::type_checker::check_comparison(cdk::binary_operation_node *const node, int lvl, bool allow_pointers) {
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  auto ok = [&](cdk::expression_node *e) {
    return e->is_typed(cdk::TYPE_INT) || e->is_typed(cdk::TYPE_DOUBLE) ||
           (allow_pointers && e->is_typed(cdk::TYPE_POINTER));
  };
  if (!ok(node->left()) || !ok(node->right())) throw std::string("cannot compare these operands");
  bool lp = node->left()->is_typed(cdk::TYPE_POINTER), rp = node->right()->is_typed(cdk::TYPE_POINTER);
  if ((lp || rp) && (node->left()->is_typed(cdk::TYPE_DOUBLE) || node->right()->is_typed(cdk::TYPE_DOUBLE)))
    throw std::string("cannot compare a pointer with a real");
  node->type(int_type());
}

void at::type_checker::do_lt_node(cdk::lt_node *const node, int lvl) {
  ASSERT_UNSPEC;
  check_comparison(node, lvl, false);
}
void at::type_checker::do_le_node(cdk::le_node *const node, int lvl) {
  ASSERT_UNSPEC;
  check_comparison(node, lvl, false);
}
void at::type_checker::do_ge_node(cdk::ge_node *const node, int lvl) {
  ASSERT_UNSPEC;
  check_comparison(node, lvl, false);
}
void at::type_checker::do_gt_node(cdk::gt_node *const node, int lvl) {
  ASSERT_UNSPEC;
  check_comparison(node, lvl, false);
}
void at::type_checker::do_eq_node(cdk::eq_node *const node, int lvl) {
  ASSERT_UNSPEC;
  check_comparison(node, lvl, true);
}
void at::type_checker::do_ne_node(cdk::ne_node *const node, int lvl) {
  ASSERT_UNSPEC;
  check_comparison(node, lvl, true);
}

//---------------------------------------------------------------------------
//     VARIABLES AND MEMORY
//---------------------------------------------------------------------------

void at::type_checker::do_variable_node(cdk::variable_node *const node, int lvl) {
  ASSERT_UNSPEC;
  auto symbol = _symtab.find(node->name());
  if (node->name() == "@" && (symbol == nullptr || symbol->depth() != _symtab.depth()))
    throw std::string("'@' is the return value only inside a non-void function");
  if (symbol == nullptr) throw std::string("undeclared variable '" + node->name() + "'");
  if (symbol->function()) throw std::string("'" + node->name() + "' is a function");
  node->type(symbol->type());
}

void at::type_checker::do_nested_variable_node(at::nested_variable_node *const node, int lvl) {
  ASSERT_UNSPEC;
  int depth = _symtab.depth() - node->depth();
  if (depth < 1) throw std::string("'@.' reaches outside the outermost function");
  auto symbol = _symtab.find(node->name(), depth);
  if (symbol == nullptr || symbol->function() || (node->name() == "@" && symbol->depth() != depth))
    throw std::string("no variable '" + node->name() + "' in the enclosing function");
  node->type(symbol->type());
}

void at::type_checker::do_rvalue_node(cdk::rvalue_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->lvalue()->accept(this, lvl);
  node->type(node->lvalue()->type());
}

void at::type_checker::do_assignment_node(cdk::assignment_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->lvalue()->accept(this, lvl + 2);
  check_for(node->rvalue(), node->lvalue()->type());
  node->type(node->lvalue()->type());
}

void at::type_checker::do_index_node(at::index_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->base()->accept(this, lvl + 2);
  if (!node->base()->is_typed(cdk::TYPE_POINTER)) throw std::string("only pointers can be indexed");
  node->index()->accept(this, lvl + 2);
  if (!node->index()->is_typed(cdk::TYPE_INT)) throw std::string("index must be an integer");
  node->type(cdk::reference_type::cast(node->base()->type())->referenced());
}

void at::type_checker::do_address_of_node(at::address_of_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->lvalue()->accept(this, lvl + 2);
  node->type(cdk::reference_type::create(4, node->lvalue()->type()));
}

void at::type_checker::do_alloc_node(at::alloc_node *const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
  if (!node->argument()->is_typed(cdk::TYPE_INT)) throw std::string("'#' needs an integer count");
  if (untyped(node)) throw std::string("'#' must be assigned to a pointer");
}

void at::type_checker::do_read_node(at::read_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(int_type());
}

//---------------------------------------------------------------------------
//     FUNCTION CALLS
//---------------------------------------------------------------------------

std::shared_ptr<at::symbol> at::type_checker::called_function(at::function_call_node *const node) {
  if (node->name() == "@") {
    auto f = _symtab.function();
    if (f == nullptr) throw std::string("'@()' used outside a function");
    return f;
  }
  auto f = _symtab.find(node->name());
  if (f == nullptr) throw std::string("undeclared function '" + node->name() + "'");
  if (!f->function()) throw std::string("'" + node->name() + "' is not a function");
  return f;
}

void at::type_checker::do_function_call_node(at::function_call_node *const node, int lvl) {
  ASSERT_UNSPEC;
  auto f = called_function(node);
  size_t given = node->arguments()->size(), wanted = f->arguments.size();
  if (given > wanted) throw std::string("too many arguments to '" + f->name() + "'");
  for (size_t ix = given; ix < wanted; ix++)
    if (f->defaults[ix] == nullptr) throw std::string("too few arguments to '" + f->name() + "'");
  for (size_t ix = 0; ix < given; ix++)
    check_for(dynamic_cast<cdk::expression_node*>(node->arguments()->node(ix)), f->arguments[ix]);
  node->type(f->type());
}
