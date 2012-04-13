//============================================================================
//---------      A T ->  T Y P E    V A L I D A T O R             ------------
//============================================================================
#include <cdk/semantics/ExpressionType.h>


#include "semantics/TypeValidator.h"
// automatically generated
#include "nodes/all.h"  
#include "semantics/Symbol.h"
#include "semantics/VariableSymbol.h"
#include "semantics/PointerType.h"

//---------------------------------------------------------------------------
// DESTROYER
at::semantics::TypeValidator::~TypeValidator() { os().flush(); }
//---------------------------------------------------------------------------
// PROCESS SEQUENCE (OK)
void at::semantics::TypeValidator::processSequence
(cdk::node::Sequence *const node, int lvl) 
{
  for (size_t i = 0; i < node->size(); i++) 
  {
    node->node(i)->accept(this, lvl);
  }
}
//---------------------------------------------------------------------------
// PROCESS SECTION NODE(OK)
void at::semantics::TypeValidator::processSectionNode
(at::node::SectionNode *const node, int lvl) 
{
   node->item()->accept(this,lvl);
}
//---------------------------------------------------------------------------
// REAL EXPRESSION(OK)
void at::semantics::TypeValidator::processDouble
(cdk::node::expression::Double *const node, int lvl) 
{
    node->type(new ExpressionType(8, ExpressionType::TYPE_DOUBLE));
}
//---------------------------------------------------------------------------
// INTEGER EXPRESSION(OK)
void at::semantics::TypeValidator::processInteger
(cdk::node::expression::Integer *const node, int lvl) 
{
    node->type(new ExpressionType(4, ExpressionType::TYPE_INT));
}
//---------------------------------------------------------------------------
// STRING EXPRESSION(OK)
void at::semantics::TypeValidator::processString
(cdk::node::expression::String *const node, int lvl) 
{
    node->type(new ExpressionType(4, ExpressionType::TYPE_STRING));
}
//---------------------------------------------------------------------------
// IDENTIFIER EXPRESSION(OK)
void at::semantics::TypeValidator::processIdentifier
(cdk::node::expression::Identifier *const node, int lvl) 
{
  const char *id = node->value().c_str();
  Symbol *symbol = _symtab.find(id);
  if (!symbol)
  { 
       throw std::string(id) + " undeclared";
       return;
  }
  else if(symbol->qualifier())
  {
        throw std::string(id) + " is a function";
  }
  else node->type(symbol->expression_type()); 
}
//---------------------------------------------------------------------------
// LEFT VALUE EXPRESSION(OK)
void at::semantics::TypeValidator::processLeftValue
(at::node::expression::LeftValue *const node, int lvl) 
{
  const char *id = node->value().c_str();
  Symbol *symbol = _symtab.find(id);
  if (!symbol)
  { 
       throw std::string(id) + " undeclared";
       return;
  }
  else if(symbol->qualifier())
  {
        throw std::string(id) + " is a function";
  }
  else node->type(symbol->expression_type()); 
}
//---------------------------------------------------------------------------
// NEG EXPRESSION(OK)
void at::semantics::TypeValidator::processNEG
(cdk::node::expression::NEG *const node, int lvl) 
{
  node->argument()->accept(this, lvl);
  if ((node->argument()->type()->name() == ExpressionType::TYPE_INT)
  || (node->argument()->type()->name() == ExpressionType::TYPE_DOUBLE))
    node->type(node->argument()->type());
  else throw std::string("integer or real expression expected");
}
//---------------------------------------------------------------------------
// NOT EXPRESSION(OK)
void at::semantics::TypeValidator::processNOT
(at::node::expression::NOT *const node, int lvl) 
{
  node->argument()->accept(this, lvl);
  if (node->argument()->type()->name() == ExpressionType::TYPE_INT)
    node->type(node->argument()->type());
  else throw std::string("integer expression expected");
}
//---------------------------------------------------------------------------
// ALLOC EXPRESSION(OK)
void at::semantics::TypeValidator::processALLOC
(at::node::expression::ALLOC *const node, int lvl) 
{
    node->argument()->accept(this, lvl);
    node->type(new ExpressionType(4, ExpressionType::TYPE_POINTER));    
}
//---------------------------------------------------------------------------
// QUESTION MARK EXPRESSION(OK)
void at::semantics::TypeValidator::processQuestionMark
(at::node::expression::QuestionMark * const node, int lvl)
{
      node->argument()->accept(this, lvl);
      node->type(node->argument()->type());
}
//---------------------------------------------------------------------------
//protected:
void at::semantics::TypeValidator::processIntOnlyExpression
(cdk::node::expression::BinaryExpression *const node, int lvl) 
{
  node->left()->accept(this, lvl+2);
  if (node->left()->type()->name() != ExpressionType::TYPE_INT)
    throw std::string("integer expression expected in binary operator (left)");

  node->right()->accept(this, lvl+2);
  if (node->right()->type()->name() != ExpressionType::TYPE_INT)
    throw std::string("integer expression expected in binary operator (right)");

  node->type(new ExpressionType(4, ExpressionType::TYPE_INT));
}

//---------------------------------------------------------------------------
//protected:
void at::semantics::TypeValidator::processIntOrRealExpression
(cdk::node::expression::BinaryExpression *const node, int lvl)
{
 // ERROR CASES (STRING EXCEPTION)
  node->left()->accept(this, lvl+2);
    if ((node->left()->type()->name() == ExpressionType::TYPE_STRING)
    || (node->left()->type()->name() == ExpressionType::TYPE_POINTER))
    {
        throw std::string("integer or real expression expected in binary operator (left)");
    }
  node->right()->accept(this, lvl+2);
    if ((node->right()->type()->name() == ExpressionType::TYPE_STRING)
     || (node->right()->type()->name() == ExpressionType::TYPE_POINTER))
    {
        throw std::string("integer or real expression expected in binary operator (right)");
    }
    // CONVERTING CASES: 
    // FIRST: (INT + INT = INT)
    // SECOND: (DOUBLE + DOUBLE = DOUBLE)
    if (node->left()->type()->name() == node->right()->type()->name())
    {
        node->type(node->left()->type());
    }
    // THIRD: (INT + DOUBLE = DOUBLE) OR (DOUBLE + INT = DOUBLE) OR (DOUBLE + DOUBLE = DOUBLE)
    else
    {
        node->type(new ExpressionType(8, ExpressionType::TYPE_DOUBLE));
    }
}
//---------------------------------------------------------------------------
//protected:
void at::semantics::TypeValidator::processAnyTypeExpression
(cdk::node::expression::BinaryExpression *const node, int lvl)
{
 // ERROR CASES (STRING EXCEPTION)
  node->left()->accept(this, lvl+2);
    if (node->left()->type()->name() == ExpressionType::TYPE_STRING)
    {
        throw std::string("integer, real or pointer expression expected in binary operator (left)");
    }
  node->right()->accept(this, lvl+2);
    if (node->right()->type()->name() == ExpressionType::TYPE_STRING)
    {
        throw std::string("integer, real or pointer expression expected in binary operator (right)");
    }
    
    // CONVERTING CASES: 
    // FIRST: (INT + INT = INT)
    // SECOND: (DOUBLE + DOUBLE = DOUBLE)
    // THIRD : (POINTER + POINTER = POINTER)
    if (node->left()->type()->name() == node->right()->type()->name())
    {
        node->type(node->left()->type());
    }
    else if((node->left()->type()->name() == ExpressionType::TYPE_POINTER)
    || (node->right()->type()->name() == ExpressionType::TYPE_POINTER))
    {
        if(((node->left()->type()->name() == ExpressionType::TYPE_DOUBLE)
        || (node->right()->type()->name() == ExpressionType::TYPE_DOUBLE)))
        {
            throw   std::string("only integer or pointer expression expected in binary operators");
        }
        else
        {
            if(node->left()->type()->name() == ExpressionType::TYPE_POINTER)
            {
                node->type(node->left()->type());
            }
            else
            {
                node->type(node->right()->type());
            }
        }
            
    }
    else
    {
        node->type(new ExpressionType(8, ExpressionType::TYPE_DOUBLE));
    }
}
//---------------------------------------------------------------------------
//protected:(OK)
void at::semantics::TypeValidator::processScalarLogicalExpression
(cdk::node::expression::BinaryExpression *const node, int lvl) 
{
  node->left()->accept(this, lvl+2);
  if (node->left()->type()->name() != ExpressionType::TYPE_INT)
    throw std::string("integer expression expected in binary logical expression (left)");

  node->right()->accept(this, lvl+2);
  if (node->right()->type()->name() != ExpressionType::TYPE_INT)
    throw std::string("integer expression expected in binary logical expression (right)");

  node->type(new ExpressionType(4, ExpressionType::TYPE_BOOLEAN));
}
//---------------------------------------------------------------------------
//protected:(OK)
void at::semantics::TypeValidator::processBooleanLogicalExpression
(cdk::node::expression::BinaryExpression *const node, int lvl) 
{
  node->left()->accept(this, lvl+2);
  if (node->left()->type()->name() != ExpressionType::TYPE_BOOLEAN)
    throw std::string("boolean expression expected in binary expression");

  node->right()->accept(this, lvl+2);
  if (node->right()->type()->name() != ExpressionType::TYPE_BOOLEAN)
    throw std::string("boolean expression expected in binary expression");

  node->type(new ExpressionType(4, ExpressionType::TYPE_BOOLEAN));
}
//---------------------------------------------------------------------------
//protected:
void at::semantics::TypeValidator::processGeneralLogicalExpression
(cdk::node::expression::BinaryExpression *const node, int lvl) 
{
  node->left()->accept(this, lvl+2);
  if ((node->left()->type()->name() != ExpressionType::TYPE_DOUBLE) 
  && (node->left()->type()->name() != ExpressionType::TYPE_INT))
    throw std::string("integer or real type expected on left side of equality operator");    
    node->right()->accept(this, lvl+2);
  if ((node->right()->type()->name() != ExpressionType::TYPE_DOUBLE) 
  && (node->right()->type()->name() != ExpressionType::TYPE_INT))
    throw std::string("integer or real type expected on right side of equality operator");
  node->type(new ExpressionType(4, ExpressionType::TYPE_BOOLEAN));
}
//---------------------------------------------------------------------------
//protected:
void at::semantics::TypeValidator::processIntLogicalExpression
(cdk::node::expression::BinaryExpression *const node, int lvl) 
{
  node->left()->accept(this, lvl+2);
  node->right()->accept(this, lvl+2);
  if ((node->left()->type()->name() != ExpressionType::TYPE_INT) 
  || (node->right()->type()->name() != ExpressionType::TYPE_INT))
    throw std::string("integer type expected on both sides of equality operator");
  node->type(new ExpressionType(4, ExpressionType::TYPE_BOOLEAN));
}
//===========================================================================
//---------      A T - S P E C I F I C    E X P R E S S I O N S  ------------
//===========================================================================
// ADD EXPRESSION (ONLY INTEGERS AND REALS) (OK) 
void at::semantics::TypeValidator::processADD(cdk::node::expression::ADD *const node, int lvl) { processAnyTypeExpression(node, lvl); }
//---------------------------------------------------------------------------
// SUBBTRACT EXPRESSION (ONLY INTEGERS AND REALS) (OK)
void at::semantics::TypeValidator::processSUB(cdk::node::expression::SUB *const node, int lvl) { processAnyTypeExpression(node, lvl); }
//---------------------------------------------------------------------------
// MULTIPLICATION EXPRESSION (ONLY INTEGERS AND REALS) (OK)
void at::semantics::TypeValidator::processMUL(cdk::node::expression::MUL *const node, int lvl) { processIntOrRealExpression(node, lvl); }
//---------------------------------------------------------------------------
// DIV EXPRESSION (ONLY INTEGERS AND REALS) (OK)
void at::semantics::TypeValidator::processDIV(cdk::node::expression::DIV *const node, int lvl) { processIntOrRealExpression(node, lvl); }
//---------------------------------------------------------------------------
// MOD EXPRESSION (ONLY INTEGERS) (OK)
void at::semantics::TypeValidator::processMOD(cdk::node::expression::MOD *const node, int lvl) { processIntOnlyExpression(node, lvl); }
//---------------------------------------------------------------------------
// POWER EXPRESSION (ONLY INTEGERS) (OK)
void at::semantics::TypeValidator::processPOWER(at::node::expression::POWER *const node, int lvl) { processIntOnlyExpression(node, lvl); }

//---------------------------------------------------------------------------
// GREAT THAN EXPRESSION (ONLY INTEGERS AND REALS)
void at::semantics::TypeValidator::processGT(cdk::node::expression::GT  *const node, int lvl) { processGeneralLogicalExpression(node, lvl);  }
//---------------------------------------------------------------------------
// GREAT OR EQUAL EXPRESSION (ONLY INTEGERS AND REALS)
void at::semantics::TypeValidator::processGE(cdk::node::expression::GE  *const node, int lvl) { processGeneralLogicalExpression(node, lvl);  }
//---------------------------------------------------------------------------
// LOWER OR EQUAL EXPRESSION (ONLY INTEGERS AND REALS)
void at::semantics::TypeValidator::processLE(cdk::node::expression::LE  *const node, int lvl) { processGeneralLogicalExpression(node, lvl);  }
//---------------------------------------------------------------------------
// LOWER THAN EXPRESSION (ONLY INTEGERS AND REALS)
void at::semantics::TypeValidator::processLT(cdk::node::expression::LT  *const node, int lvl) { processGeneralLogicalExpression(node, lvl);  }
//---------------------------------------------------------------------------
// EQUAL EXPRESSION (ONLY INTEGERS AND REALS)
void at::semantics::TypeValidator::processEQ(cdk::node::expression::EQ  *const node, int lvl) { processGeneralLogicalExpression(node, lvl);  }
//---------------------------------------------------------------------------
// NOT EQUAL EXPRESSION (ONLY INTEGERS AND REALS)
void at::semantics::TypeValidator::processNE(cdk::node::expression::NE  *const node, int lvl) { processGeneralLogicalExpression(node, lvl);  }
//---------------------------------------------------------------------------
// LOGICAL AND EXPRESSION (ONLY INTEGERS) (OK)
void at::semantics::TypeValidator::processAND
(at::node::expression::AND * const node, int lvl)
{ 
        processIntLogicalExpression(node, lvl);  
}
//---------------------------------------------------------------------------
// LOGICAL OR EXPRESSION (ONLY INTEGERS) (OK)
void at::semantics::TypeValidator::processOR
(at::node::expression::OR * const node, int lvl)
{ 
        processIntLogicalExpression(node, lvl);  
}
//===========================================================================
//---------      A T - S P E C I F I C    N O D E S              ------------
//===========================================================================
// PROCESS DECLARATION NODE
void at::semantics::TypeValidator::processDeclarationNode
(at::node::DeclarationNode *const node, int lvl) 
{
  try 
  {
    node->lvalue()->accept(this, lvl+4);
    _errors = true;     // flag para read on
    node->rvalue()->accept(this, lvl+4);
    _errors = false;    // flag para read off

    // Tratamento do Read     
    if (node->rvalue()->type()->name() == ExpressionType::TYPE_UNSPEC)
    {
        if((node->lvalue()->type()->name() != ExpressionType::TYPE_STRING))
        {
            node->rvalue()->type(node->lvalue()->type());
        }
        else 
            throw std::string("wrong types in declaration");
    }
    
    // Cast de Int para Double
    else if((node->lvalue()->type()->name() == ExpressionType::TYPE_DOUBLE)
    && (node->rvalue()->type()->name() == ExpressionType::TYPE_INT))
    {
      node->rvalue()->type(new ExpressionType(8,ExpressionType::TYPE_DOUBLE));
    }
    
    // Tratamento de Ponteiros
    else if(node->rvalue()->type()->name() == ExpressionType::TYPE_POINTER)
    {
        node->rvalue()->type(node->lvalue()->type());
    }
    // Tratamento de Ponteiros
    else if(node->lvalue()->type()->name() == ExpressionType::TYPE_POINTER)
    {
        // Verificação se o ponteiro foi inicializado com valor != de zero
        if(node->rvalue()->type()->name() == ExpressionType::TYPE_INT)
        {
         cdk::node::expression::Simple<int>* simple = (cdk::node::expression::Simple<int>*) node->rvalue();
            if(simple->value() != 0)
                throw  std::string("a pointer only can be initialized with zero");
        }
        else
            throw std::string("wrong type in declaration to a pointer");
    }
    // Caso normal
    else if((node->lvalue()->type()->name() != node->rvalue()->type()->name()))
          throw std::string("wrong types in declaration");
  }
  catch (std::string s) 
  { 
    throw s; 
  }
}
//---------------------------------------------------------------------------
// FUNCTION NODE (OK)
void at::semantics::TypeValidator::processFunctionNode
(at::node::FunctionNode *const node, int lvl) 
{
  // teste ao valor de retorno por omissão
  if(node->literal() != NULL)
  {
    node->literal()->accept(this,lvl);
    if(node->type()->name() != node->literal()->type()->name())
    {
        throw std::string("wrong default return value of function");
    }
  }
}
//---------------------------------------------------------------------------
// FUNCTION DECLARATION NODE (OK)
void at::semantics::TypeValidator::processFunctionDeclNode
(at::node::FunctionDeclNode *const node, int lvl) 
{
  // teste ao valor de retorno por omissão
  if(node->literal() != NULL)
  {
    node->literal()->accept(this,lvl);
    if(node->type()->name() != node->literal()->type()->name())
        throw std::string("wrong default return value of function");
  }
}

//---------------------------------------------------------------------------
// FOR NODE (OK)
void at::semantics::TypeValidator::processForNode
(at::node::ForNode *const node, int lvl) 
{
  try 
  {
    node->init()->accept(this, lvl);
    node->condition()->accept(this, lvl);
    node->incrmt()->accept(this, lvl);
  }
  catch (std::string s) { throw s; }
}
//---------------------------------------------------------------------------
// IF NODE (OK)
void at::semantics::TypeValidator::processIfNode
(at::node::IfNode *const node, int lvl) 
{
    node->condition()->accept(this, lvl);
}
//---------------------------------------------------------------------------
// INCLUSIVE NODE (OK)
void at::semantics::TypeValidator::processInclusiveNode
(at::node::InclusiveNode *const node, int lvl) 
{
    node->condition()->accept(this, lvl);
}
//---------------------------------------------------------------------------
// IF ELSE NODE (OK)
void at::semantics::TypeValidator::processIfElseNode
(at::node::IfElseNode *const node, int lvl) 
{
    node->condition()->accept(this, lvl);
}
//===========================================================================
//---------      A T - S P E C I F I C    E X P R E S S I O N S  ------------
//===========================================================================
// ATRIBUTION NODE EXPRESSION (OK)
void at::semantics::TypeValidator::processAtribNode
(at::node::expression::AtribNode *const node, int lvl) 
{
  try 
  {
    node->lvalue()->accept(this, lvl+4);
    _errors = true; // Flag para Read On
    node->rvalue()->accept(this, lvl+4);
   _errors = false; // Flag para Read Off
   
   // Tratamento do Read
    if (node->rvalue()->type()->name() == ExpressionType::TYPE_UNSPEC)
    {
        node->rvalue()->type(node->lvalue()->type());
    }
    
    // Cast de Int para Double
    else if((node->lvalue()->type()->name() == ExpressionType::TYPE_DOUBLE)
    && (node->rvalue()->type()->name() == ExpressionType::TYPE_INT))
    {
      node->rvalue()->type(new ExpressionType(8,ExpressionType::TYPE_DOUBLE));
    }
    
    // Tratamento de ponteiros
    else if(node->lvalue()->type()->name() == ExpressionType::TYPE_POINTER)
    {
        PointerType* pt = (PointerType*) node->lvalue()->type();
        // Se o r_value for do tipo pointer
        if(node->rvalue()->type()->name() == ExpressionType::TYPE_POINTER)
        {
            // O Nó fica com a informação apenas de um inteiro
            node->type(new ExpressionType(4, ExpressionType::TYPE_INT));
            // O r_value fica com a informação do left value
            node->rvalue()->type(node->lvalue()->type());
            return;            
        }
        
        else if(pt->PointerName() != node->rvalue()->type()->name())
        {
            throw std::string("wrong types in assignment");
        }
        else
        {
            node->type(node->lvalue()->type());
            return;            
        }
    }
    // Tratamento comum                        
    else if(node->lvalue()->type()->name() != node->rvalue()->type()->name())
    {
      throw std::string("wrong types in assignment");
    }
    node->type(node->lvalue()->type());
  }
  catch (std::string s) 
  { 
    throw s; 
  }
}
//---------------------------------------------------------------------------
// INPUT ARGUMENT NODE FUNCTION EXPRESSION (OK)
void at::semantics::TypeValidator::processInputArgumentNode
(at::node::InputArgumentNode *const node, int lvl) 
{
    if(node->literal() != NULL)
    {
        node->literal()->accept(this, lvl);
        if((node->type()->name() == ExpressionType::TYPE_DOUBLE)
        && (node->literal()->type()->name() == ExpressionType::TYPE_INT))
        {
            node->literal()->type(node->type());
        }
        else if(node->type()->name() != node->literal()->type()->name())
        {
            throw std::string(" wrong type in argument function");
        }
    }
}
//---------------------------------------------------------------------------
// CALL FUNCTION EXPRESSION (OK)
void at::semantics::TypeValidator::processCallFunction
(at::node::expression::CallFunction *const node, int lvl) 
{
     const char *id = node->id()->value().c_str();
    // Verificação se a função chamada é a função actual
     if(*id == '@')
     {
         id = &_context.at(_context.size()-1)[0].name()[0];
     }
     Symbol *symbol = _symtab.find(id);
     try
     {
        if (!symbol) 
        {
            throw std::string(id) + " undeclared";
        }
        if(symbol->qualifier()==0)
        {
            throw std::string(id) + " is a variable, not a function";
        }
        FunctionSymbol* fs = (FunctionSymbol*) symbol;
        if(node->vars() != NULL)
        {
            if(node->vars()->size() > fs->types_size())
            {
                throw std::string(id) + " called with many arguments";
            }
            else
            {
                    node->vars()->accept(this, lvl);
            }
        }
        for(size_t i = 0; i < fs->literal_size(); i++)
        {
            fs->lit_at(i)->accept(this, lvl);
        }   
        node->type(symbol->expression_type()); 
    }
    catch(std::string s)
    {
        throw s;
    }
}
//---------------------------------------------------------------------------
// NESTED VAR EXPRESSION (OK)
void at::semantics::TypeValidator::processNestedVar
(at::node::expression::NestedVar *const node, int lvl) 
{
  const char *id = node->Id()->value().c_str();
  if(node->Nesting() > _context.size()-1)
  {
    throw std::string(id) + " nesting level out of bounds";
    return;
  }
  Symbol *symbol = _symtab.find(id,*level - _context.size()-1);
  if (!symbol)
  { 
       throw std::string(id) + " undeclared";
       return;
  }
  else if(symbol->qualifier())
  {
        throw std::string(id) + " is a function";
        return;
  }
 node->type(symbol->expression_type()); 
}
//---------------------------------------------------------------------------
// ACCESS POINTER EXPRESSION
void at::semantics::TypeValidator::processAccessPointer
(at::node::expression::AccessPointer *const node, int lvl) 
{
    if(node->Id() != NULL)    
    {
        node->Id()->accept(this, lvl);
        node->type(node->Id()->type());
    }
    PointerType *pt = (PointerType*) node->type();
    if(node->Exprs() != NULL)
    {
        node->Exprs()->accept(this, lvl);
    }
    if(node->Node() != NULL)
    {
       node->type(new PointerType(pt->NestLvl()-1,pt->PointerName()));
        node->Node()->type(node->type());
        node->Node()->accept(this, lvl);
    }
}
//---------------------------------------------------------------------------
// READ NODE EXPRESSION (OK)
void at::semantics::TypeValidator::processReadNode
(at::node::expression::ReadNode *const node, int lvl) 
{
   if(_errors)
   {
    // Tipo definido pelo nó acima
    node->type(new ExpressionType());  
   }
   else  
   node->type(new ExpressionType(4, ExpressionType::TYPE_INT));
}
//---------------------------------------------------------------------------
// POSITION EXPRESSION (OK)
void at::semantics::TypeValidator::processPosition
(at::node::expression::Position *const node, int lvl)
{
    node->expression()->accept(this, lvl);
    if(node->isPointer())
    {
        PointerType *pt=  (PointerType*) node->expression()->type();
        
        if(pt->PointerName() == ExpressionType::TYPE_DOUBLE)
            node->type(new ExpressionType(8,ExpressionType::TYPE_DOUBLE));
            
        if(pt->PointerName() == ExpressionType::TYPE_INT)           
            node->type(new ExpressionType(4,ExpressionType::TYPE_INT));
            
        if(pt->PointerName() == ExpressionType::TYPE_STRING)           
            node->type(new ExpressionType(4,ExpressionType::TYPE_STRING));
    }
    else
    node->type(node->expression()->type());
}
//---------------------------------------------------------------------------
// PRINT NODE (OK)
void at::semantics::TypeValidator::processPrintNode
(at::node::PrintNode * const node, int lvl)
{
  try 
  {
        node->argument()->accept(this, lvl+2);
        if ((node->argument()->type()->name() != ExpressionType::TYPE_STRING) 
        && (node->argument()->type()->name() != ExpressionType::TYPE_DOUBLE) 
        && (node->argument()->type()->name() != ExpressionType::TYPE_INT))
        {
            if(node->argument()->type()->name() == ExpressionType::TYPE_BOOLEAN)
            {
                node->argument()->type(new ExpressionType(4, ExpressionType::TYPE_INT));
            }
            else
                throw std::string("Invalid type expression for print operation");
        }
  }
  catch(std::string s)
  {
        throw s;
  }
}
//---------------------------------------------------------------------------
// TRASH NODE (OK)
void at::semantics::TypeValidator::processTrashNode
(at::node::TrashNode * const node, int lvl)
{
  node->expression()->accept(this, lvl);
}
//---------------------------------------------------------------------------
//                          T H E    E N D
//---------------------------------------------------------------------------
 
