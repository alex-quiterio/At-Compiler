#ifndef __AT_SEMANTICS_SEMANTICPROCESSOR_H__
#define __AT_SEMANTICS_SEMANTICPROCESSOR_H__

#include <string>
#include <iostream>

/* include node forward declarations */
#define __NODE_DECLARATIONS_ONLY__
#include "nodes/all.h"  // automatically generated
#undef __NODE_DECLARATIONS_ONLY__

//!
//! Print nodes as XML elements to the output stream.
//!
class SemanticProcessor {
  //! The output stream
  std::ostream &_os;

protected:
  SemanticProcessor(std::ostream &os = std::cout) :
    _os(os) {
  }
  inline std::ostream &os() {
    return _os;
  }

public:
  virtual ~SemanticProcessor() {
  }

public:
  virtual void processNode(cdk::node::Node * const node, int lvl) = 0;
  virtual void processNil(cdk::node::Nil * const node, int lvl) = 0;
  virtual void processData(cdk::node::Data * const node, int lvl) = 0;
  virtual void processComposite(cdk::node::Composite * const node, int lvl) = 0;
  virtual void processSequence(cdk::node::Sequence * const node, int lvl) = 0;

public:
  template<typename T> void processSimple(cdk::node::expression::Simple<T> * const node, int lvl) {
    throw 42;
  }
  virtual void processInteger(cdk::node::expression::Integer * const node, int lvl) = 0;
  virtual void processDouble(cdk::node::expression::Double * const node, int lvl) = 0;
  virtual void processString(cdk::node::expression::String * const node, int lvl) = 0;
  virtual void processIdentifier(cdk::node::expression::Identifier * const node, int lvl) = 0;

public:
  virtual void
      processUnaryExpression(cdk::node::expression::UnaryExpression * const node, int lvl) = 0;
  virtual void processNEG(cdk::node::expression::NEG * const node, int lvl) = 0;
  virtual void processNOT(at::node::expression::NOT * const node, int lvl) = 0;
  virtual void processALLOC(at::node::expression::ALLOC * const node, int lvl) = 0;
  virtual void processQuestionMark(at::node::expression::QuestionMark * const node, int lvl) = 0;

public:
  virtual void processBinaryExpression(cdk::node::expression::BinaryExpression * const node,
                                       int lvl) = 0;
  virtual void processADD(cdk::node::expression::ADD * const node, int lvl) = 0;
  virtual void processOR(at::node::expression::OR * const node, int lvl) = 0;
  virtual void processPOWER(at::node::expression::POWER * const node, int lvl) = 0;
  virtual void processAND(at::node::expression::AND * const node, int lvl) = 0;
  virtual void processSUB(cdk::node::expression::SUB * const node, int lvl) = 0;
  virtual void processMUL(cdk::node::expression::MUL * const node, int lvl) = 0;
  virtual void processDIV(cdk::node::expression::DIV * const node, int lvl) = 0;
  virtual void processMOD(cdk::node::expression::MOD * const node, int lvl) = 0;
  virtual void processLT(cdk::node::expression::LT * const node, int lvl) = 0;
  virtual void processLE(cdk::node::expression::LE * const node, int lvl) = 0;
  virtual void processGE(cdk::node::expression::GE * const node, int lvl) = 0;
  virtual void processGT(cdk::node::expression::GT * const node, int lvl) = 0;
  virtual void processNE(cdk::node::expression::NE * const node, int lvl) = 0;
  virtual void processEQ(cdk::node::expression::EQ * const node, int lvl) = 0;
  virtual void processReadNode(at::node::expression::ReadNode * const node, int lvl) = 0;
  virtual void processCallFunction(at::node::expression::CallFunction * const node, int lvl) = 0;
  virtual void processNestedVar(at::node::expression::NestedVar * const node, int lvl) = 0;
  virtual void processAccessPointer(at::node::expression::AccessPointer * const node, int lvl) = 0;
  virtual void processPosition(at::node::expression::Position * const node, int lvl) = 0;

public:
  virtual void processLeftValue(at::node::expression::LeftValue * const node, int lvl) = 0;

public:
  virtual void processFileNode(at::node::FileNode * const node, int lvl) = 0;
  virtual void processFunctionNode(at::node::FunctionNode * const node, int lvl) = 0;
  virtual void processFunctionDeclNode(at::node::FunctionDeclNode * const node, int lvl) = 0;
  virtual void processInputArgumentNode(at::node::InputArgumentNode * const node, int lvl) = 0;
  virtual void processPrintNode(at::node::PrintNode * const node, int lvl) = 0;
  virtual void processForNode(at::node::ForNode * const node, int lvl) = 0;
  virtual void processDeclarationNode(at::node::DeclarationNode * const node, int lvl) = 0;  
  virtual void processAtribNode(at::node::expression::AtribNode * const node, int lvl) = 0;
  virtual void processBodyNode(at::node::BodyNode * const node, int lvl) = 0;
  virtual void processSectionNode(at::node::SectionNode * const node, int lvl) = 0;
  virtual void processTrashNode(at::node::TrashNode * const node, int lvl) = 0;
  virtual void processIfNode(at::node::IfNode * const node, int lvl) = 0;
  virtual void processInclusiveNode(at::node::InclusiveNode * const node, int lvl) = 0;
  virtual void processIfElseNode(at::node::IfElseNode * const node, int lvl) = 0;
  virtual void processNextNode(at::node::NextNode *const node, int lvl) = 0;
  virtual void processStopNode(at::node::StopNode *const node, int lvl) = 0;
  virtual void processReturnNode(at::node::ReturnNode *const node, int lvl) = 0;
  

}; // class

#endif
