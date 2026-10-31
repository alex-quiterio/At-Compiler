#include "targets/frame_size_calculator.h"
#include ".auto/all_nodes.h"  // automatically generated

//---------------------------------------------------------------------------
//     NODES THAT CAN HOLD DECLARATIONS
//---------------------------------------------------------------------------

void at::frame_size_calculator::do_sequence_node(cdk::sequence_node *const node, int lvl) {
  for (auto n : node->nodes())
    if (n) n->accept(this, lvl + 2);
}

void at::frame_size_calculator::do_variable_declaration_node(at::variable_declaration_node *const node, int lvl) {
  _localsize += node->type()->size();
}

void at::frame_size_calculator::do_body_node(at::body_node *const node, int lvl) {
  if (node->initial()) node->initial()->accept(this, lvl + 2);
  node->sections()->accept(this, lvl + 2);
  if (node->final()) node->final()->accept(this, lvl + 2);
}

void at::frame_size_calculator::do_section_node(at::section_node *const node, int lvl) {
  node->block()->accept(this, lvl + 2);
}

void at::frame_size_calculator::do_block_node(at::block_node *const node, int lvl) {
  node->declarations()->accept(this, lvl + 2);
  node->instructions()->accept(this, lvl + 2);
}

void at::frame_size_calculator::do_if_node(at::if_node *const node, int lvl) {
  node->block()->accept(this, lvl + 2);
  if (node->elseblock()) node->elseblock()->accept(this, lvl + 2);
}

void at::frame_size_calculator::do_if_else_node(at::if_else_node *const node, int lvl) {
  node->thenblock()->accept(this, lvl + 2);
  node->elseblock()->accept(this, lvl + 2);
}

void at::frame_size_calculator::do_for_node(at::for_node *const node, int lvl) {
  if (node->declarations()) node->declarations()->accept(this, lvl + 2);
  node->block()->accept(this, lvl + 2);
}

//---------------------------------------------------------------------------
//     EVERYTHING ELSE: NO LOCALS
//---------------------------------------------------------------------------

// a nested function's locals live in its own frame
void at::frame_size_calculator::do_function_definition_node(at::function_definition_node *const node, int lvl) {}
void at::frame_size_calculator::do_function_declaration_node(at::function_declaration_node *const node, int lvl) {}

void at::frame_size_calculator::do_file_node(at::file_node *const node, int lvl) {}
void at::frame_size_calculator::do_evaluation_node(at::evaluation_node *const node, int lvl) {}
void at::frame_size_calculator::do_print_node(at::print_node *const node, int lvl) {}
void at::frame_size_calculator::do_next_node(at::next_node *const node, int lvl) {}
void at::frame_size_calculator::do_stop_node(at::stop_node *const node, int lvl) {}
void at::frame_size_calculator::do_return_node(at::return_node *const node, int lvl) {}
void at::frame_size_calculator::do_nil_node(cdk::nil_node *const node, int lvl) {}
void at::frame_size_calculator::do_data_node(cdk::data_node *const node, int lvl) {}
void at::frame_size_calculator::do_integer_node(cdk::integer_node *const node, int lvl) {}
void at::frame_size_calculator::do_double_node(cdk::double_node *const node, int lvl) {}
void at::frame_size_calculator::do_string_node(cdk::string_node *const node, int lvl) {}
void at::frame_size_calculator::do_balanced3_node(cdk::balanced3_node *const node, int lvl) {}
void at::frame_size_calculator::do_posit3_node(cdk::posit3_node *const node, int lvl) {}
void at::frame_size_calculator::do_takum3_node(cdk::takum3_node *const node, int lvl) {}
void at::frame_size_calculator::do_unary_minus_node(cdk::unary_minus_node *const node, int lvl) {}
void at::frame_size_calculator::do_unary_plus_node(cdk::unary_plus_node *const node, int lvl) {}
void at::frame_size_calculator::do_not_node(cdk::not_node *const node, int lvl) {}
void at::frame_size_calculator::do_add_node(cdk::add_node *const node, int lvl) {}
void at::frame_size_calculator::do_sub_node(cdk::sub_node *const node, int lvl) {}
void at::frame_size_calculator::do_mul_node(cdk::mul_node *const node, int lvl) {}
void at::frame_size_calculator::do_div_node(cdk::div_node *const node, int lvl) {}
void at::frame_size_calculator::do_mod_node(cdk::mod_node *const node, int lvl) {}
void at::frame_size_calculator::do_lt_node(cdk::lt_node *const node, int lvl) {}
void at::frame_size_calculator::do_le_node(cdk::le_node *const node, int lvl) {}
void at::frame_size_calculator::do_ge_node(cdk::ge_node *const node, int lvl) {}
void at::frame_size_calculator::do_gt_node(cdk::gt_node *const node, int lvl) {}
void at::frame_size_calculator::do_ne_node(cdk::ne_node *const node, int lvl) {}
void at::frame_size_calculator::do_eq_node(cdk::eq_node *const node, int lvl) {}
void at::frame_size_calculator::do_and_node(cdk::and_node *const node, int lvl) {}
void at::frame_size_calculator::do_or_node(cdk::or_node *const node, int lvl) {}
void at::frame_size_calculator::do_variable_node(cdk::variable_node *const node, int lvl) {}
void at::frame_size_calculator::do_rvalue_node(cdk::rvalue_node *const node, int lvl) {}
void at::frame_size_calculator::do_assignment_node(cdk::assignment_node *const node, int lvl) {}
void at::frame_size_calculator::do_address_of_node(at::address_of_node *const node, int lvl) {}
void at::frame_size_calculator::do_alloc_node(at::alloc_node *const node, int lvl) {}
void at::frame_size_calculator::do_function_call_node(at::function_call_node *const node, int lvl) {}
void at::frame_size_calculator::do_index_node(at::index_node *const node, int lvl) {}
void at::frame_size_calculator::do_nested_variable_node(at::nested_variable_node *const node, int lvl) {}
void at::frame_size_calculator::do_power_node(at::power_node *const node, int lvl) {}
void at::frame_size_calculator::do_read_node(at::read_node *const node, int lvl) {}
