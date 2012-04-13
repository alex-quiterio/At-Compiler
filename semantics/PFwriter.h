#ifndef __AT_SEMANTICS_PFWRITER_H__
#define __AT_SEMANTICS_PFWRITER_H__

#include <string>
#include <iostream>
#include <stack>
#include <vector>
#include <cdk/semantics/SymbolTable.h>
#include <cdk/generators/Postfix.h>


#include "semantics/SemanticProcessor.h"
#include "semantics/Symbol.h"
#include "semantics/FunctionSymbol.h"
#include "semantics/VariableSymbol.h"

namespace at {
  namespace semantics {

    //!
    //! Traverse syntax tree and generate the corresponding nasm code.
    //!
    class PFwriter: public virtual SemanticProcessor {
      cdk::semantics::SymbolTable<Symbol> &_symtab;
      cdk::generator::Postfix &_pf;      
      std::stack<int> _contextNEXT;
      std::stack<int> _contextSTOP;
      std::stack<int> _contextOffSet;
      std::stack<int> _return_body;
      std::stack<int> _return_func;      
      
      int _lbl;
      std::vector<FunctionSymbol*> _nestlvl;
      std::vector<std::streambuf*> _back_up;
      std::vector<std::ostringstream*> _stream;
      bool _return_flag;
      int _lvltab;

    public:
      PFwriter(std::ostream &os, cdk::semantics::SymbolTable<Symbol> &symtab, cdk::generator::Postfix &pf) :
        SemanticProcessor(os), _symtab(symtab), _pf(pf), _lbl(0), _nestlvl(),_back_up(),_stream(),_return_flag(true), _lvltab(0)
        {}

    public:
      ~PFwriter() {
        os().flush();
      }

    private:
      std::string mklbl(int lbl);
      inline int getNextOffSet(int size);
      std::string getFunctionName();

    public:
      void processNode(cdk::node::Node * const node, int lvl) {
      }
      void processNil(cdk::node::Nil * const node, int lvl) {
      }
      void processData(cdk::node::Data * const node, int lvl) {
      }
      void processComposite(cdk::node::Composite * const node, int lvl) {
      }
      void processSequence(cdk::node::Sequence * const node, int lvl);

    public:
      template<typename T> void processSimple(cdk::node::expression::Simple<T> * const node,
                                              int lvl) {
      }
      void processInteger(cdk::node::expression::Integer * const node, int lvl);
      void processDouble(cdk::node::expression::Double * const node, int lvl);
      void processString(cdk::node::expression::String * const node, int lvl);
      void processIdentifier(cdk::node::expression::Identifier * const node, int lvl);

    public:
      void processUnaryExpression(cdk::node::expression::UnaryExpression * const node, int lvl) {
      }
      void processNEG(cdk::node::expression::NEG * const node, int lvl);
      void processNOT(at::node::expression::NOT * const node, int lvl);
      void processALLOC(at::node::expression::ALLOC * const node, int lvl);
      void processQuestionMark(at::node::expression::QuestionMark * const node, int lvl);
 protected:
      void processRealIntExp(cdk::node::expression::BinaryExpression * const node, int lvl, int Operator);
      void processRealIntScalar(cdk::node::expression::BinaryExpression * const node, int lvl, int Operator);  
      void processExpression(cdk::node::expression::BinaryExpression * const node, int lvl, int Operator);
    public:
      void processBinaryExpression(cdk::node::expression::BinaryExpression * const node, int lvl) {
      }
      void processADD(cdk::node::expression::ADD * const node, int lvl);
      void processOR(at::node::expression::OR * const node, int lvl);
      void processPOWER(at::node::expression::POWER * const node, int lvl);
      void processAND(at::node::expression::AND * const node, int lvl);
      void processSUB(cdk::node::expression::SUB * const node, int lvl);
      void processMUL(cdk::node::expression::MUL * const node, int lvl);
      void processDIV(cdk::node::expression::DIV * const node, int lvl);
      void processMOD(cdk::node::expression::MOD * const node, int lvl);
      void processLT(cdk::node::expression::LT * const node, int lvl);
      void processLE(cdk::node::expression::LE * const node, int lvl);
      void processGE(cdk::node::expression::GE * const node, int lvl);
      void processGT(cdk::node::expression::GT * const node, int lvl);
      void processNE(cdk::node::expression::NE * const node, int lvl);
      void processEQ(cdk::node::expression::EQ * const node, int lvl);
      void processReadNode(at::node::expression::ReadNode * const node, int lvl);
      void processCallFunction(at::node::expression::CallFunction * const node, int lvl);
      void processNestedVar(at::node::expression::NestedVar * const node, int lvl);
      void processAccessPointer(at::node::expression::AccessPointer * const node, int lvl);
      void processPosition(at::node::expression::Position * const node, int lvl);

    public:
      void processLeftValue(at::node::expression::LeftValue * const node, int lvl);

    public:
      void processFileNode(at::node::FileNode * const node, int lvl);
      void processFunctionNode(at::node::FunctionNode * const node, int lvl);
      void processTrashNode(at::node::TrashNode * const node, int lvl);
      void processFunctionDeclNode(at::node::FunctionDeclNode * const node, int lvl);
      void processPrintNode(at::node::PrintNode * const node, int lvl);
      void processForNode(at::node::ForNode * const node, int lvl);
      void processDeclarationNode(at::node::DeclarationNode * const node, int lvl);
      void processAtribNode(at::node::expression::AtribNode * const node, int lvl);
      void processIfNode(at::node::IfNode * const node, int lvl);
      void processInputArgumentNode(at::node::InputArgumentNode * const node, int lvl);
      void processBodyNode(at::node::BodyNode * const node, int lvl);
      void processSectionNode(at::node::SectionNode * const node, int lvl);
      void processInclusiveNode(at::node::InclusiveNode * const node, int lvl);
      void processIfElseNode(at::node::IfElseNode * const node, int lvl);
      void processNextNode(at::node::NextNode * const node, int lvl);
      void processStopNode(at::node::StopNode * const node, int lvl);
      void processReturnNode(at::node::ReturnNode * const node, int lvl);

    }; // class

  } // namespace semantics
} // namespace at

#endif
