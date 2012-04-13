#include <string>
#include <sstream>

#include "semantics/PFwriter.h"
#include "semantics/TypeValidator.h"
#include "semantics/ArgumentCounter.h"
#include "semantics/PointerType.h"
#include "nodes/all.h"  // automatically generated

//---------------------------------------------------------------------------
//                     THIS IS THE EVALUATOR'S DEFINITION
//---------------------------------------------------------------------------
//private:
std::string at::semantics::PFwriter::mklbl(int lbl) 
{
  std::ostringstream oss;
  if (lbl < 0)
    oss << ".L" << -lbl;
  else
    oss << "_L" << lbl;
  return oss.str();
}
//---------------------------------------------------------------------------
//private:
// Para primitir ter funções com o mesmo nome em secções distintas
int desambiguity = 0;
// Constroi os nomes para o Nasm de todas as funções expt. main
std::string at::semantics::PFwriter::getFunctionName() 
{
  std::ostringstream oss;
  for(size_t i = 0; i < _nestlvl.size()-1; i++)
  {
    oss << _nestlvl.at(i)->name() << "$";
  }
    oss << _nestlvl.at(_nestlvl.size()-1)->name() << desambiguity++;
  return oss.str();
}
//---------------------------------------------------------------------------
//private:
// retorna o proximo offset
int at::semantics::PFwriter::getNextOffSet
(int size)
{
        return _contextOffSet.top() -= size;
}
//---------------------------------------------------------------------------
// PROCESS FILE NODE (OK)
void at::semantics::PFwriter::processFileNode
(at::node::FileNode * const node,int lvl) 
{
  node->statements()->accept(this, lvl);
}
//---------------------------------------------------------------------------
// PROCESS SEQUENCE (OK)
void at::semantics::PFwriter::processSequence
(cdk::node::Sequence * const node, int lvl) 
{
  for (size_t i = 0; i < node->size(); i++) 
  {
    node->node(i)->accept(this, lvl + 2);
  }
}
//===========================================================================
//---------      A T - S P E C I F I C    E X P R E S S I O N S  ------------
//===========================================================================
// PROCESS INTEGER EXPRESSION (OK) 
void at::semantics::PFwriter::processInteger
(cdk::node::expression::Integer * const node,int lvl) 
{
    int lbl1;
  if(!_nestlvl.empty())
  {
     if(node->type()->name() != ExpressionType::TYPE_DOUBLE)
        _pf.INT(node->value());
    // Cast para Double
    else
    {
          _pf.RODATA();
          _pf.ALIGN();
          _pf.LABEL(mklbl(lbl1 = ++_lbl));
          _pf.DOUBLE(node->value());
          _pf.TEXT();
          _pf.ADDR(mklbl(lbl1));
          _pf.LOAD2(); 
     }
  }
  else 
  {
    if(node->type()->name() != ExpressionType::TYPE_DOUBLE)
    _pf.CONST(node->value()); 
    else
    _pf.DOUBLE(node->value());
  }
}
//---------------------------------------------------------------------------
// PROCESS DOUBLE EXPRESSION (OK)
void at::semantics::PFwriter::processDouble
(cdk::node::expression::Double * const node,int lvl) 
{
      int lbl1;
      if(!_nestlvl.empty())
      {
          _pf.RODATA();
          _pf.ALIGN();
          _pf.LABEL(mklbl(lbl1 = ++_lbl));
          _pf.DOUBLE(node->value());
          _pf.TEXT();
          _pf.ADDR(mklbl(lbl1));
          _pf.LOAD2(); 
      }
      else
      _pf.DOUBLE(node->value());
}
//---------------------------------------------------------------------------
// PROCESS STRING EXPRESSION (OK)
void at::semantics::PFwriter::processString
(cdk::node::expression::String * const node,int lvl) 
{
  int lbl1;
  _pf.RODATA(); 
  _pf.ALIGN(); 
  _pf.LABEL(mklbl(lbl1 = ++_lbl));
  _pf.STR(node->value()); 
  _pf.TEXT();
  if(!_nestlvl.empty())
      _pf.ADDR(mklbl(lbl1)); 
}
//---------------------------------------------------------------------------
// PROCESS IDENTIFIER EXPRESSION (OK)
void at::semantics::PFwriter::processIdentifier
(cdk::node::expression::Identifier * const node,int lvl) 
{
  const char *id = node->value().c_str();
  VariableSymbol* symbol = (VariableSymbol*)_symtab.find(id);
  bool out = false;
  if (symbol) 
  {
        if(!_nestlvl.empty())
        {
          if(symbol->OffSet() == 0)
          {
             out = true;
          }
        }
        // out or inside a function
        if(out) 
        {
            _pf.ADDR(id);
        }
        else    
        {            
            if(symbol->NestLvl() == _nestlvl.size())
            {
                _pf.LOCAL(symbol->OffSet());
            }
            else
            {
                int nest = _nestlvl.size() - symbol->NestLvl()-1;
                _pf.LOCAL(0);
                _pf.LOAD();
                while(nest > 0)
                {
                    _pf.LOAD();
                    nest--;
                }
                _pf.INT(symbol->OffSet());
                _pf.ADD();            
            }
        }
        // 4 or 8 bytes' value
        if(symbol->type() == ExpressionType::TYPE_DOUBLE)
        _pf.LOAD2();
        else _pf.LOAD();
           
            
  }
}
//---------------------------------------------------------------------------
// PROCESS LEFT VALUE EXPRESSION (OK)
void at::semantics::PFwriter::processLeftValue
(at::node::expression::LeftValue *const node, int lvl) 
{
  const char *id = node->value().c_str();
  VariableSymbol* symbol = (VariableSymbol*)_symtab.find(id);
  bool out = false;
  if (symbol) 
  {
        if(!_nestlvl.empty())
        {
          if(symbol->OffSet() == 0)
          {
             out = true;
          }
        }
        // out or inside a function
        if(out) {_pf.ADDR(id);}
        else    
        {
            if(symbol->NestLvl() == _nestlvl.size())
            {
                _pf.LOCAL(symbol->OffSet());
            }
            else
            {
                int nest = _nestlvl.size() - symbol->NestLvl()-1;
                _pf.LOCAL(0);
                _pf.LOAD();
                while(nest > 0)
                {
                    _pf.LOAD();
                    nest--;
                }
                _pf.INT(symbol->OffSet());
                _pf.ADD();            
            }
        }
  }
}
//===========================================================================
//---------      A T - S P E C I F I C    E X P R E S S I O N S  ------------
//---------              R E A L   O R   I N T E G E R    O P S  ------------
//===========================================================================
// protected
// PROCESS ANY REAL OR INTEGER ARITMETIC EXPRESSIONS 
void at::semantics::PFwriter::processRealIntExp
(cdk::node::expression::BinaryExpression * const node, int lvl, int Operator) 
{
  int _select_operation = 0;
  try 
  {
    TypeValidator *tv = new TypeValidator(os(), _symtab, _nestlvl, &_lvltab);
    node->accept(tv, lvl);
    // CASE ONE = INT OP INT = INT
    if((node->left()->type()->name() == ExpressionType::TYPE_INT) 
    && (node->right()->type()->name() == ExpressionType::TYPE_INT))
    {
        node->left()->accept(this, lvl);
        node->right()->accept(this, lvl);
        _select_operation = 1;
    }
    // CASE TWO = DOUBLE OP INT = DOUBLE
    else if((node->left()->type()->name() == ExpressionType::TYPE_DOUBLE)
    &&(node->right()->type()->name() == ExpressionType::TYPE_INT))
    {
        node->left()->accept(this, lvl);
        node->right()->accept(this, lvl);
        _pf.I2D();
        
    }
    // CASE  = INT OP DOUBLE = DOUBLE
    else if((node->left()->type()->name() == ExpressionType::TYPE_INT)
    &&(node->right()->type()->name() == ExpressionType::TYPE_DOUBLE))
    {
        node->left()->accept(this, lvl);
        _pf.I2D();
        node->right()->accept(this, lvl);
    }
    // OTHERWISE = DOUBLE OP DOUBLE = DOUBLE
    else
    {
        node->left()->accept(this, lvl);
        node->right()->accept(this, lvl);
    }
    
    if(!_select_operation) // OPERATION DOUBLE CASE
    {
     switch (Operator)
        {
            case 1:
            {
                _pf.DMUL();
                break;
            }
            case 2:
            {
                _pf.DDIV();
                break;
            }
        }
     }
     else // OPERATION INT CASE
     {
     switch (Operator)
        {
            case 1:
            {
                _pf.MUL();
                break;
            }
            case 2:
            {
                _pf.DIV();
                break;
            }
        }
      }
  }
  catch (std::string s) 
  {
    std::cerr << "FATAL ERROR at " << node->lineno() << " -> " << s << std::endl;
    return;
  }
}
//---------------------------------------------------------------------------
// protected
// PROCESS ANY REAL OR INTEGER SCALAR EXPRESSIONS
void at::semantics::PFwriter::processRealIntScalar
(cdk::node::expression::BinaryExpression * const node, int lvl, int Operator) 
{
  try 
  {
    // CASE ONE = INT OP INT 
    if((node->left()->type()->name() == ExpressionType::TYPE_INT) 
    && (node->right()->type()->name() == ExpressionType::TYPE_INT))
    {
        node->left()->accept(this, lvl);
        node->right()->accept(this, lvl);
    }
    // CASE TWO = DOUBLE OP INT
    else if((node->left()->type()->name() == ExpressionType::TYPE_DOUBLE)
    &&(node->right()->type()->name() == ExpressionType::TYPE_INT))
    {
        node->left()->accept(this, lvl);
        node->right()->accept(this, lvl);
        _pf.I2D();
        _pf.DCMP();
        _pf.INT(0);
    }
    // CASE  = INT OP DOUBLE 
    else if((node->left()->type()->name() == ExpressionType::TYPE_INT)
    &&(node->right()->type()->name() == ExpressionType::TYPE_DOUBLE))
    {
        node->left()->accept(this, lvl);
        _pf.I2D();
        node->right()->accept(this, lvl);
        _pf.DCMP();
        _pf.INT(0);
    }
    else
    {
        node->left()->accept(this, lvl);
        node->right()->accept(this, lvl);
        _pf.DCMP();
        _pf.INT(0);
    }
    switch (Operator)
    {
        case 1:
        {
            _pf.EQ();
            break;
        }
        case 2:
        {
            _pf.LT();
            break;
        }
        case 3:
        {
            _pf.LE();
            break;
        }
        case 4:
        {
           _pf.GT();
           break;
        }
        case 5:
        {
           _pf.GE();
           break;
        }
        case 6:
        {
           _pf.NE();
           break;
        }     
    }
  }
  catch (std::string s) 
  {
    std::cerr << "FATAL ERROR at " << node->lineno() << " -> " << s << std::endl;
    return;
  }
} 
//---------------------------------------------------------------------------
// protected
// PROCESS ANY REAL OR INTEGER OR POINTER EXPRESSIONS
void at::semantics::PFwriter::processExpression
(cdk::node::expression::BinaryExpression * const node, int lvl, int Operator) 
{
  int _select_operation = 0;
  try 
  {
    TypeValidator *tv = new TypeValidator(os(), _symtab, _nestlvl, &_lvltab);
    node->accept(tv, lvl);
    // CASE ONE = INT OP INT = INT
    if((node->left()->type()->name() == ExpressionType::TYPE_INT) 
    && (node->right()->type()->name() == ExpressionType::TYPE_INT))
    {
        node->left()->accept(this, lvl);
        node->right()->accept(this, lvl);
        _select_operation = 1;
    }
    // Operações de ponteiros e inteiros (Caso Um)
    else if((node->left()->type()->name() == ExpressionType::TYPE_INT) 
    && (node->right()->type()->name() == ExpressionType::TYPE_POINTER))
    {
        PointerType *pt = (PointerType*) node->right()->type();
        if(pt->PointerName() == ExpressionType::TYPE_DOUBLE && (pt->NestLvl()==1))
        {   
            _pf.INT(8);
        }
        else
            _pf.INT(4);
            
            node->left()->accept(this, lvl);
            _pf.MUL();
            node->right()->accept(this, lvl);
            _select_operation = 1;
            
    }
    // Operações de ponteiros e inteiros (Caso Dois)
    else if((node->left()->type()->name() == ExpressionType::TYPE_POINTER) 
    && (node->right()->type()->name() == ExpressionType::TYPE_INT))
    {
        PointerType *pt = (PointerType*) node->left()->type();
        if(pt->PointerName() == ExpressionType::TYPE_DOUBLE && (pt->NestLvl()==1))
        {   
            _pf.INT(8);
        }
        else
            _pf.INT(4);
            
            node->right()->accept(this, lvl);
            _pf.MUL();
            node->left()->accept(this, lvl);
            _select_operation = 1;
            
    }
    // Operações só de ponteiros
    else if((node->left()->type()->name() == ExpressionType::TYPE_POINTER) 
    && (node->right()->type()->name() == ExpressionType::TYPE_POINTER))
    {
            node->right()->accept(this, lvl);
            node->left()->accept(this, lvl);
            _select_operation = 1;       
    }
    // CASE TWO = DOUBLE OP INT = DOUBLE
    else if((node->left()->type()->name() == ExpressionType::TYPE_DOUBLE)
    &&(node->right()->type()->name() == ExpressionType::TYPE_INT))
    {
        node->left()->accept(this, lvl);
        node->right()->accept(this, lvl);
        _pf.I2D();
        
    }
    // CASE  = INT OP DOUBLE = DOUBLE
    else if((node->left()->type()->name() == ExpressionType::TYPE_INT)
    &&(node->right()->type()->name() == ExpressionType::TYPE_DOUBLE))
    {
        node->left()->accept(this, lvl);
        _pf.I2D();
        node->right()->accept(this, lvl);
    }
    // OTHERWISE = DOUBLE OP DOUBLE = DOUBLE
    else
    {
        node->left()->accept(this, lvl);
        node->right()->accept(this, lvl);
    }
    
    if(!_select_operation) // OPERATION DOUBLE CASE
    {
     switch (Operator)
        {
            case 1:
            {
                _pf.DADD();
                break;
            }
            case 2:
            {
                _pf.DSUB();
                break;
            }
        }
     }
     else // OPERATION INT OR POINTER CASE
     {
     switch (Operator)
        {
            case 1:
            {
                _pf.ADD();
                break;
            }
            case 2:
            {
                _pf.SUB();
                break;
            }
        }
      }
  }
  catch (std::string s) 
  {
    std::cerr << "FATAL ERROR at " << node->lineno() << " -> " << s << std::endl;
    return;
  }
}
//---------------------------------------------------------------------------
// PROCESS ADD EXPRESSION (REAL & INTEGERS & POINTERS) 
void at::semantics::PFwriter::processADD
(cdk::node::expression::ADD * const node, int lvl) 
{
  processExpression(node, lvl, 1);
}
//---------------------------------------------------------------------------
// PROCESS SUB EXPRESSION (REAL & INTEGERS & POINTERS) 
void at::semantics::PFwriter::processSUB
(cdk::node::expression::SUB * const node, int lvl) 
{
  processExpression(node, lvl, 2);
}
//---------------------------------------------------------------------------
// PROCESS MUL EXPRESSION (ONLY REAL & INTEGERS) (OK)
void at::semantics::PFwriter::processMUL
(cdk::node::expression::MUL * const node, int lvl) 
{
  processRealIntExp(node, lvl, 1);
}
//---------------------------------------------------------------------------
// PROCESS DIV EXPRESSION (ONLY REAL & INTEGERS) (OK)
void at::semantics::PFwriter::processDIV
(cdk::node::expression::DIV * const node, int lvl) 
{
  processRealIntExp(node, lvl, 2);

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// PROCESS EQUAL EXPRESSION (ONLY REAL & INTEGERS) (OK)
void at::semantics::PFwriter::processEQ
(cdk::node::expression::EQ * const node, int lvl) 
{
      processRealIntScalar(node, lvl, 1);
}
//---------------------------------------------------------------------------
// PROCESS LESS THAN EXPRESSION (ONLY REAL & INTEGERS) (OK)
void at::semantics::PFwriter::processLT
(cdk::node::expression::LT * const node, int lvl) 
{
      processRealIntScalar(node, lvl, 2);
}
//---------------------------------------------------------------------------
// PROCESS LESS OR EQUAL EXPRESSION (ONLY REAL & INTEGERS)(OK)
void at::semantics::PFwriter::processLE
(cdk::node::expression::LE * const node, int lvl) 
{
      processRealIntScalar(node, lvl, 3);
}
//---------------------------------------------------------------------------
// PROCESS GREAT THAN EXPRESSION (ONLY REAL & INTEGERS)(OK)
void at::semantics::PFwriter::processGT
(cdk::node::expression::GT * const node, int lvl) 
{
      processRealIntScalar(node, lvl, 4);
}
//---------------------------------------------------------------------------
// PROCESS GREAT OR EQUAL EXPRESSION (ONLY REAL & INTEGERS)(OK)
void at::semantics::PFwriter::processGE
(cdk::node::expression::GE * const node, int lvl) 
{
      processRealIntScalar(node, lvl, 5);
}
//---------------------------------------------------------------------------
// PROCESS NOT EQUAL EXPRESSION (ONLY REAL & INTEGERS)(OK)
void at::semantics::PFwriter::processNE
(cdk::node::expression::NE * const node, int lvl) 
{
     processRealIntScalar(node, lvl, 6);
}
//---------------------------------------------------------------------------
// PROCESS NEG EXPRESSION (INTEGERS OR REALS) (OK)
void at::semantics::PFwriter::processNEG
(cdk::node::expression::NEG * const node, int lvl) 
{
  try 
  {
    TypeValidator *tv = new TypeValidator(os(), _symtab,_nestlvl, &_lvltab);
    node->accept(tv, lvl);
    node->argument()->accept(this, lvl);
    if(node->argument()->type()->name() == ExpressionType::TYPE_DOUBLE)
    {
     _pf.DNEG(); 
    }
    else _pf.NEG();
  }
  catch (std::string s) 
  {
    std::cerr << "FATAL ERROR at " << node->lineno() << " -> " << s << std::endl;
    return;
  }
}
//===========================================================================
//---------      A T - S P E C I F I C    E X P R E S S I O N S  ------------
//---------              O N L Y   I N T E G E R    O P S        ------------
//===========================================================================
// PROCESS POWER EXPRESSION (ONLY INTEGERS)(OK)
void at::semantics::PFwriter::processPOWER
(at::node::expression::POWER * const node, int lvl) 
{
  int lbl1, lbl2;
  try 
  {
      TypeValidator *tv = new TypeValidator(os(), _symtab, _nestlvl, &_lvltab);
      node->accept(tv, lvl);
      
      node->right()->accept(this,lvl);
      node->left()->accept(this, lvl);
      
      _pf.LABEL(mklbl(lbl1 = ++_lbl));
      _pf.SWAP();
      _pf.INT(1);
      _pf.SUB();
      _pf.DUP();
      _pf.JZ(mklbl(lbl2 = ++_lbl));
      _pf.SWAP();
      node->left()->accept(this, lvl);
      _pf.MUL();  
      _pf.JMP(mklbl(lbl1));
      _pf.LABEL(mklbl(lbl2));
      _pf.TRASH(4);
  }
  catch (std::string s) 
  {
    std::cerr << "FATAL ERROR at " << node->lineno() << " -> " << s << std::endl;
    return;
  }
}
//---------------------------------------------------------------------------
// PROCESS AND EXPRESSION (ONLY INTEGERS) (OK)
void at::semantics::PFwriter::processAND
(at::node::expression::AND * const node, int lvl) 
{
    int lbl1, lbl2, lbl3;
  try 
  {
    TypeValidator *tv = new TypeValidator(os(), _symtab, _nestlvl, &_lvltab);
    node->accept(tv, lvl);
    
    node->left()->accept(this, lvl);
    _pf.JZ(mklbl(lbl1 = ++_lbl));
    node->right()->accept(this, lvl);
    _pf.JZ(mklbl(lbl1));
    _pf.JMP(mklbl(lbl2 = ++_lbl));
    _pf.LABEL(mklbl(lbl1));
    _pf.INT(0);
    _pf.JMP(mklbl(lbl3 = ++_lbl));
    _pf.LABEL(mklbl(lbl2));
    _pf.INT(1);
    _pf.LABEL(mklbl(lbl3));
  }
  catch (std::string s) 
  {
    std::cerr << "FATAL ERROR at " << node->lineno() << " -> " << s << std::endl;
    return;
  }  
}
//---------------------------------------------------------------------------
// PROCESS OR EXPRESSION (ONLY INTEGERS)(OK)
void at::semantics::PFwriter::processOR
(at::node::expression::OR * const node, int lvl) 
{
    int lbl1, lbl2, lbl3;
  try 
  {
    TypeValidator *tv = new TypeValidator(os(), _symtab, _nestlvl, &_lvltab);
    node->accept(tv, lvl);
    
    node->left()->accept(this, lvl);
    _pf.JNZ(mklbl(lbl1 = ++_lbl));
    node->right()->accept(this, lvl);
    _pf.JNZ(mklbl(lbl1));
    _pf.JMP(mklbl(lbl2 = ++_lbl));
    _pf.LABEL(mklbl(lbl1));
    _pf.INT(1);
    _pf.JMP(mklbl(lbl3 = ++_lbl));
    _pf.LABEL(mklbl(lbl2));
    _pf.INT(0);
    _pf.LABEL(mklbl(lbl3));  }
  catch (std::string s) 
  {
    std::cerr << "FATAL ERROR at " << node->lineno() << " -> " << s << std::endl;
    return;
  }
}
//---------------------------------------------------------------------------
// PROCESS MOD EXPRESSION (ONLY INTEGERS) (OK)
void at::semantics::PFwriter::processMOD
(cdk::node::expression::MOD * const node, int lvl) 
{
  try 
  {
    TypeValidator *tv = new TypeValidator(os(), _symtab, _nestlvl, &_lvltab);
    node->accept(tv, lvl);
    
    node->left()->accept(this, lvl);
    node->right()->accept(this, lvl);
    _pf.MOD();
  }
  catch (std::string s) 
  {
    std::cerr << "FATAL ERROR at " << node->lineno() << " -> " << s << std::endl;
    return;
  }
}
//---------------------------------------------------------------------------
// PROCESS NOT EXPRESSION (ONLY INTEGERS) (OK)
void at::semantics::PFwriter::processNOT
(at::node::expression::NOT * const node, int lvl) 
{
  try 
  {
    TypeValidator *tv = new TypeValidator(os(), _symtab, _nestlvl, &_lvltab);
    node->accept(tv, lvl);
    node->argument()->accept(this, lvl); 
    _pf.INT(0);
    _pf.EQ();

  }
  catch (std::string s) 
  {
    std::cerr << "FATAL ERROR at " << node->lineno() << " -> " << s << std::endl;
    return;
  }
}
//===========================================================================
//---------      A T - S P E C I F I C    E X P R E S S I O N S  ------------
//---------                     G E N E R A L                    ------------
//===========================================================================
// PROCESS ALLOC EXPRESSION (OK)
void at::semantics::PFwriter::processALLOC
(at::node::expression::ALLOC * const node, int lvl) 
{    
    PointerType* pt = (PointerType*) node->type();
    if((pt->PointerName() == ExpressionType::TYPE_DOUBLE) && (pt->NestLvl() == 1))
    {
        _pf.INT(8);
    }
    else 
    {
        _pf.INT(4);
    }
    node->argument()->accept(this, lvl); 
    _pf.MUL();
    _pf.ALLOC();
    _pf.SP();
}
//---------------------------------------------------------------------------
// PROCESS QUESTION MARK EXPRESSION (OK)
void at::semantics::PFwriter::processQuestionMark
(at::node::expression::QuestionMark * const node, int lvl) 
{
    node->argument()->accept(this, lvl);
}
//---------------------------------------------------------------------------
// PROCESS READ NODE EXPRESSION (OK)
void at::semantics::PFwriter::processReadNode
(at::node::expression::ReadNode * const node, int lvl) 
{
  if(node->type()->name() == ExpressionType::TYPE_DOUBLE)
  {
    _pf.CALL("readd");  
    _pf.DPUSH();
  }
  else
  {
    _pf.CALL("readi");
    _pf.PUSH();
  }
}
//---------------------------------------------------------------------------
// PROCESS NESTED VAR EXPRESSION
void at::semantics::PFwriter::processNestedVar
(at::node::expression::NestedVar * const node, int lvl) 
{
    const char *id = node->Id()->value().c_str();
    int context = _lvltab - _nestlvl.size()-1;     
    try
    {   
        TypeValidator *tv = new TypeValidator(os(), _symtab, _nestlvl, &_lvltab);
        node->accept(tv, lvl);
        VariableSymbol* symb = (VariableSymbol*) _symtab.find(id,context);
        _pf.LOCAL(0);
        for(size_t i = 0; i < node->Nesting(); i++)
        {
           _pf.LOAD();
        }

        _pf.INT(symb->OffSet());
        _pf.ADD();            
    }
    catch(std::string s)
    {
        std::cerr << "FATAL ERROR at " << node->lineno() << " -> " << s << std::endl;
        return;
    }
}
//---------------------------------------------------------------------------
// PROCESS ACCESS POINTER EXPRESSION 
void at::semantics::PFwriter::processAccessPointer
(at::node::expression::AccessPointer * const node, int lvl) 
{

    PointerType *pt = (PointerType*) node->type();
    if(node->Id() != NULL)
    { 
        node->Id()->accept(this, lvl);
    }
    if(node->Exprs() != NULL)
    {

        node->Exprs()->accept(this ,lvl);
        node->type(new PointerType(pt->NestLvl()-1,pt->PointerName()));
        if(pt->NestLvl() == 0)
        {
            if((pt->PointerName() == ExpressionType::TYPE_DOUBLE))
            {   
              _pf.INT(8);
            }
            else
            {
              _pf.INT(4);
            }
        }        
        else _pf.INT(4);
        _pf.MUL();
        _pf.ADD();
    }  
    if(node->Node() != NULL)
    {
        node->Node()->type(node->type());
        node->Node()->accept(this ,lvl);
        if(pt->NestLvl() != 0)
         _pf.LOAD();        
    }
}
//---------------------------------------------------------------------------
// PROCESS POSITION EXPRESSION (OK)
// O valor para ponteiros ou para os @.@.tIdentifier
void at::semantics::PFwriter::processPosition
(at::node::expression::Position* const node, int lvl)
{
    node->expression()->accept(this, lvl);
    if(node->isPointer())
    {
        PointerType *pt = (PointerType*) node->expression()->type();
        if(pt->PointerName() == ExpressionType::TYPE_DOUBLE)
        {
            _pf.LOAD2();
        }
        else
            _pf.LOAD();
    }
    else
    {
        if(node->expression()->type()->name() == ExpressionType::TYPE_DOUBLE)
        {
            _pf.LOAD2();
        }
        else
            _pf.LOAD();
     }
}
//---------------------------------------------------------------------------
// PROCESS ATRIB NODE EXPRESSION (OK)
void at::semantics::PFwriter::processAtribNode
(at::node::expression::AtribNode * const node,int lvl) 
{
  try 
  {
    TypeValidator *tv = new TypeValidator(os(), _symtab, _nestlvl, &_lvltab);
    node->accept(tv, lvl);
     // determine the new value
    node->rvalue()->accept(this, lvl);   
    
    if(node->rvalue()->type()->name() == ExpressionType::TYPE_DOUBLE) 
    {
       // duplicate the value double in the stack        
        _pf.SP();
        _pf.LOAD2();
    }
    // duplicate the value in the stack        
    else _pf.DUP();
    // where to store the value                        
    node->lvalue()->accept(this, lvl);   
    if(node->lvalue()->type()->name() == ExpressionType::TYPE_DOUBLE)
    {
        _pf.STORE2();
    }
    else 
    {
        // Se o r_value for do tipo pointer então estamos em 4 bytes
        if(node->rvalue()->type()->name() == ExpressionType::TYPE_POINTER)
        {
            _pf.STORE();
        }
        // caso o teste de cima falhe verifica-se o tipo do ponteiro 
        else if(node->lvalue()->type()->name() == ExpressionType::TYPE_POINTER)
        { 
            PointerType *pt = (PointerType*) node->lvalue()->type();
            if(pt->PointerName() == ExpressionType::TYPE_DOUBLE)
            {
                _pf.STORE2();
            }
            else _pf.STORE();
        }
        else _pf.STORE(); // store the value at address
    }
  }
  catch (std::string s) 
  {
    std::cerr << "FATAL ERROR at " << node->lineno() << " -> " << s << std::endl;
    return;
  }
}
//---------------------------------------------------------------------------
// PROCESS CALL FUNCTION EXPRESSION (OK)
void at::semantics::PFwriter::processCallFunction
(at::node::expression::CallFunction *const node, int lvl) 
{      
        const char *id = node->id()->value().c_str();

        // Verificação se a função chamada é a função actual
        if(*id == '@')
        {
            id = &_nestlvl.at(_nestlvl.size()-1)[0].name()[0];
        }
        // Identificação do simbolo função testado previamente no
        // Type Validator
        Symbol *symbol = _symtab.find(id);
        FunctionSymbol* fs = (FunctionSymbol*) symbol;
        
        // Se os argumentos de entrada existirem...
        if(node->vars() != NULL)
        {
            int size = node->vars()->size();
            int total_size = fs->literal_size();
            
            // Se tivermos o numero total de argumentos 
            // basta colocá-los na pilha.
            int delta = total_size - size;
            int aux = 0;
            // Os restantes elementos se faltarem serão 
            // recebidos pelos literais de omissão            
            while(delta > aux)
            {
               fs->lit_at(total_size - aux - 1)->accept(this, lvl);
               aux++;
            }
            for(int i = 1; i <= size; i++)
            {
               node->vars()->node(size - i)->accept(this, lvl);
            }
         }
         else
         {
            // caso os argumentos de entrada sejam ignorados
            // utilizam-se os de omissão
            for(int i = fs->literal_size()-1; i >= 0; i--)
            {
                fs->lit_at(i)->accept(this,lvl);
            }
         }
         // Chamada à função
         _pf.CALL(fs->NasmName());
         // trash dos argumentos de entrada
         _pf.TRASH(fs->SizeOfArgs());
         if(fs->type() == ExpressionType::TYPE_DOUBLE)
             _pf.DPUSH();
         // So se trata do valor de retorno se esteve for != de VOID
         else if(fs->type() != ExpressionType::TYPE_VOID)
             _pf.PUSH();
}
//===========================================================================
//---------      A T - S P E C I F I C    N O D E S              ------------
//===========================================================================
// PROCESS INPUT ARGS OF A FUNCTION NODE (OK)
void at::semantics::PFwriter::processInputArgumentNode
(at::node::InputArgumentNode * const node, int lvl)
{
    const char *id = node->id()->value().c_str(); 
    int off_set = _contextOffSet.top();
    try
    {
        TypeValidator *tv = new TypeValidator(os(), _symtab,_nestlvl, &_lvltab);
        node->accept(tv, lvl);
        FunctionSymbol *fs = (FunctionSymbol*) _nestlvl.at(_nestlvl.size() - 1);
        if(!_symtab.insert(id,new VariableSymbol(node->type(),id,off_set, _nestlvl.size())))
        {
            throw std::string(id) + " redeclared";
        }
        // avaliacao do literal (se existir)
        if(node->literal() != NULL)
        {
                // inserção nos atributos do simbolo Função
                fs->literal_push(node->literal());
        }
         fs->type_push(node->type());
         _contextOffSet.top() += node->type()->size();
     }
     catch(std::string s)
     {
        std::cerr << "FATAL ERROR at " << node->lineno() << " -> " << s << std::endl;  
        return;
     }
}
//---------------------------------------------------------------------------
// PROCESS FUNCTION NODE
void at::semantics::PFwriter::processFunctionNode
(at::node::FunctionNode *const node, int lvl) 
{         
    /*
     * Stream para escrever funções internas
     */
    std::ostringstream *oss = new std::ostringstream(std::ostringstream::out);
    // id da funcao
    const char *id = node->id()->value().c_str(); 
    /*
     * Criação do Simbolo Function e inserção
     * do mesmo na stack _nestlvl.
     */
    FunctionSymbol *fs = new FunctionSymbol(node->type(),id);
    _nestlvl.push_back(fs);
    std::string name = getFunctionName();
    /*
     * Para o Assembly é necessário que as funções tenham 
     * nomes distintos, para tal têm um nome associado no seu
     * simbolo
     */
    fs->SetNasmName(name);
    
    // Procura-se um simbolo com este id
    Symbol* symbol = _symtab.find(id);
    // se existir teremos de testar:
    if(symbol)
    {
        // Se não existir localmente pode inserir-se na tab.
        if(!_symtab.findLocal(id,symbol))
        {
            _symtab.insert(id,fs);
        }
        else
        {
            // Verifica-se se é função
            if(symbol->qualifier() == 1)
            {
                FunctionSymbol* func = (FunctionSymbol*) symbol;
                // Teste se já foi escrita anteriormente
                if(func->written())
                {
    std::cerr << "FATAL ERROR at " << node->lineno() << " -> function " << id << " redeclared" <<std::endl;  
                    return;
                }
                else
                {
                    func->written(true);
                    fs = func;
                    
                }
            }
            else
            {
     std::cerr << "FATAL ERROR at " << node->lineno() << " -> function " << id << " redeclared" <<std::endl;
                return;
            }
        }
     }
     // insere-se tranquilamente c.c
     else   _symtab.insert(id,fs);
     
         /*
     * Contador do tamanho ocupado pelos Argumentos ou variavies
     * visitadas
     */
    _symtab.push();
    _lvltab++;
    ArgumentCounter *Ac = new ArgumentCounter();
    node->vars()->accept(Ac, lvl);
    fs->SetSizeArgs(Ac->getArgsSize());

    /*
     * Se a função for interna a uma outra função
     * utiliza-se o stream criado para colocar
     * o output temporariamente
     */
    if(_nestlvl.size() > 1) 
    {
        std::streambuf* back_up = os().rdbuf();
        _back_up.push_back(back_up);
        _stream.push_back(oss);
        os().rdbuf(oss->rdbuf());
    }
    try
    {  
      /*
       * Teste ao retorno por omissão da função em causa
       */
       TypeValidator *tv = new TypeValidator(os(), _symtab,_nestlvl, &_lvltab);        
       node->accept(tv,lvl);
    }
    catch(std::string s)
    {
        std::cerr << "FATAL ERROR at " << node->lineno() << " -> " << s << std::endl;
        return;
    }
    
    /*
     * OFF-SET a 8 para argumentos de entrada da função
     */
    _contextOffSet.push(8);      
    node->vars()->accept(this,lvl); 
    /*
     * Geração de código NASM para a função
     */
    _pf.TEXT();
    _pf.ALIGN();
    // funcao main (at)
    if(id[0] == 'a' && id[1] == 't') 
    {
       _pf.GLOBL("_main", _pf.FUNC());
       _pf.LABEL("_main");
       fs->SetNasmName("_main");
    }
    else
    {
        // se for public 
       if(node->qualifier() == 271)
       _pf.GLOBL(fs->NasmName(), _pf.FUNC());
       _pf.LABEL(fs->NasmName());
    }

    Ac->Reset();
    // Contador de variaveis locais 
    node->accept(Ac,lvl);     
    /* o tamanho de reserva = tamanho total das variaveis locais
     * mais o tamanho do tipo de retorno
     */
    int size = Ac->getArgsSize() + node->type()->size();
    // OFF-SET a 0 para variavies locais
    _contextOffSet.top() = 0;
    _pf.ENTER(size); 
    //insercao do valor de retorno de omissão se não for void
    if(node->type()->name() != ExpressionType::TYPE_VOID)
    {    
         _symtab.insert("@",new VariableSymbol(node->type(),"@",-size, _nestlvl.size()));
         if(node->literal() != NULL)
         {
             node->literal()->accept(this,lvl);
             if(node->type()->name() == ExpressionType::TYPE_DOUBLE)
             {
                _pf.LOCAL(-size);
                _pf.STORE2();
             }
             else _pf.LOCA(-size);
         }
    }
    // geração do codigo do corpo da função
    node->body()->accept(this,lvl);
    
    //retorno da função
    if(node->type()->name() != ExpressionType::TYPE_VOID)
    {
         if(node->type()->name() == ExpressionType::TYPE_DOUBLE)
         {
             _pf.LOCAL(-size);
             _pf.LOAD2();
             // guardar o valor de retorno no registo acumulador
             _pf.DPOP();
         }
         else 
         {
             _pf.LOCV(-size);
             // guardar o valor de retorno no registo acumulador
             _pf.POP();
         }
    }
    // Saida da função
    _pf.LEAVE();
    // voltar ao SP anterior
    _pf.RET(); 
    _contextOffSet.pop();
    /* 
     * Caso seja a Função Contentora mais acima na hierarquia
     * e tenha pelo menos uma função interna, descarrega todo
     * o conteúdo dos streams no seu próprio stream e limpa a 
     * stack para posterior utilização
     */
    if(_nestlvl.size() == 1 && !_stream.empty())
    {
      for(size_t i = 0; i < _stream.size(); i++)
      {
          os() << _stream.at(i)->str();
      }
      _stream.clear();
    }  
    /*
     * Caso seja uma função interna após a utilização
     * da sua stream, redirecciona o stream anterior
     * para o canal de saida.
     */
    else if( _nestlvl.size() > 1)
    {     
       os().rdbuf(_back_up.at(_back_up.size()-1));
       _back_up.pop_back();
    }

    // Contexto de função eliminado
     _nestlvl.pop_back();      
     // contexto de tabela de simbolos eliminado
     _symtab.pop();   
     _symtab.pop();   
     _lvltab--;
     _lvltab--;     
}
//---------------------------------------------------------------------------
// PROCESS FUNCTION DECLARATION NODE 
void at::semantics::PFwriter::processFunctionDeclNode
(at::node::FunctionDeclNode *const node, int lvl) 
{
    // id da funcao
    const char *id = node->id()->value().c_str(); 
    /*
     * Criação do Simbolo Function e inserção
     * do mesmo na stack _nestlvl.
     */
    if(!_symtab.find(id))
    {
        FunctionSymbol *fs = new FunctionSymbol(node->type(),id, false);
        _symtab.insert(id,fs);
        _nestlvl.push_back(fs);
        std::string name = getFunctionName();
        /*
         * Para o Assembly é necessário que as funções tenham 
         * nomes distintos, para tal têm um nome associado no seu
         * simbolo
         */
        fs->SetNasmName(name);
        /*
         * Contador do tamanho ocupado pelos Argumentos ou variavies
         * visitadas
         */
        ArgumentCounter *Ac = new ArgumentCounter();
        node->vars()->accept(Ac, lvl);
        fs->SetSizeArgs(Ac->getArgsSize());

        _contextOffSet.push(8);      
        node->vars()->accept(this,lvl); 
        _contextOffSet.pop();

        _nestlvl.pop_back();
        try
        {  
          /*
           * Teste ao retorno por omissão da função em causa
           */
           TypeValidator *tv = new TypeValidator(os(), _symtab,_nestlvl, &_lvltab);        
           node->accept(tv,lvl);
           if(node->qualifier() == 270)
           {

           }
        }
        catch(std::string s)
        {
            std::cerr << "FATAL ERROR at " << node->lineno() << " -> " << s << std::endl;
            return;
        }
    }
    else
    {
    std::cerr << "FATAL ERROR at " << node->lineno() << " -> function " << id << " redeclared" <<std::endl;
    return;
    }
}
//---------------------------------------------------------------------------
// PROCESS Declaration NODE 
void at::semantics::PFwriter::processDeclarationNode
(at::node::DeclarationNode * const node,int lvl) 
{
   const char *id = node->lvalue()->value().c_str();
   // caso estejamos dentro de uma funcao
  if(!_nestlvl.empty()) 
  {   
    if (_symtab.insert(id,new VariableSymbol(node->type(),id,getNextOffSet(node->type()->size()),_nestlvl.size())))
    { 
            // Caso o r_value nao exista nada faz
            if(node->rvalue() == NULL) 
            {
                return;
            }
            // Após a inserção faz um visit para etiquetar o lvalue e rvalue
            try 
            {
                TypeValidator *tv = new TypeValidator(os(), _symtab, _nestlvl, &_lvltab);
                node->accept(tv, lvl);
            }
            catch (std::string s) 
            {
                std::cerr << "FATAL ERROR at " << node->lineno() << " -> " << s << std::endl;
                return;
            }
            // Visita o r_value NASM
            node->rvalue()->accept(this,lvl); 
            if(node->type()->name() == ExpressionType::TYPE_DOUBLE)
            {
                _pf.LOCAL(_contextOffSet.top());
                _pf.STORE2();
            }
            else 
            {
                _pf.LOCA(_contextOffSet.top());
            }
     }
     else
     {
          std::cerr << "FATAL ERROR at " << node->lineno() << " -> " << id << " redeclared" <<std::endl;
          return;
     }
  }

  else if(_symtab.insert(id, new VariableSymbol(node->type(), id, 0, _nestlvl.size()))) 
  {   
        if(node->rvalue() != NULL)
        {   
             try 
             {
                TypeValidator *tv = new TypeValidator(os(), _symtab, _nestlvl, &_lvltab);
                node->accept(tv, lvl);
             }
             catch (std::string s) 
             {
                std::cerr << "FATAL ERROR at " << node->lineno() << " -> " << s << std::endl;
                return;
             }
             if((node->type()->name() == ExpressionType::TYPE_STRING))
             {
                node->rvalue()->accept(this,lvl);
                if(node->is_constant())
                    _pf.RODATA();
                else _pf.DATA(); 
                _pf.ALIGN();
                if(node->qualifier()) // public qualifier
                _pf.GLOBL(id,_pf.OBJ());
                _pf.LABEL(id); 
                _pf.ID(mklbl(_lbl));
                _pf.TEXT(); 
             }
             else if((node->type()->name() == ExpressionType::TYPE_DOUBLE))
             {
                if(node->is_constant())
                    _pf.RODATA();
                else _pf.DATA(); 
                _pf.ALIGN();
                if(node->qualifier()) // public qualifier
                _pf.GLOBL(id,_pf.OBJ());
                _pf.LABEL(id);
                node->rvalue()->accept(this, lvl);
                _pf.TEXT(); 
             }
             else if(node->type()->name() == ExpressionType::TYPE_INT)
             {
                if(node->is_constant())
                _pf.RODATA();
                else _pf.DATA(); 
                _pf.ALIGN();
                if(node->qualifier()) // public qualifier
                _pf.GLOBL(id,_pf.OBJ());
                _pf.LABEL(id); 
                node->rvalue()->accept(this,lvl);
                _pf.TEXT();
             }   
             else if(node->type()->name() == ExpressionType::TYPE_POINTER)
             {
                PointerType * pt = (PointerType*) node->type();
                _pf.DATA(); 
                _pf.ALIGN();
                if(node->qualifier()) // public qualifier
                _pf.GLOBL(id,_pf.OBJ());
                _pf.LABEL(id); 
                if((pt->NestLvl() == 1) && (pt->PointerName() == ExpressionType::TYPE_DOUBLE))
                _pf.DOUBLE(0);
                else
                _pf.CONST(0);
                _pf.TEXT();
             }
        }
        else
        {       if(node->is_use())
                {
                        _pf.EXTRN(id);
                        return;
                }
             else if((node->type()->name() == ExpressionType::TYPE_DOUBLE))
             {
                _pf.DATA(); 
                _pf.ALIGN();
                if(node->qualifier()) // public qualifier
                _pf.GLOBL(id,_pf.OBJ());
                _pf.LABEL(id); 
                _pf.DOUBLE(0);
                _pf.TEXT();
             }
             else
             { 
               _pf.DATA(); 
                _pf.ALIGN();
                if(node->qualifier()) // public qualifier
                _pf.GLOBL(id,_pf.OBJ());
                _pf.LABEL(id); 
                _pf.CONST(0);
                _pf.TEXT();
             }
        }
  }
  else
  {
        std::cerr << "FATAL ERROR at " << node->lineno() << " -> " << id << " redeclared" <<std::endl;
        return;
  }
}
//---------------------------------------------------------------------------
// PROCESS SECTION NODE (OK)
void at::semantics::PFwriter::processSectionNode
(at::node::SectionNode * const node, int lvl) 
{
    if(node->qualifier() == 0)
    {
        _symtab.push();
        _lvltab++;
        
    }
    node->item()->accept(this,lvl+2);
    if(node->qualifier() == 0)
    {
        _symtab.pop();
        _lvltab--;
    }
}
//---------------------------------------------------------------------------
// PROCESS BODY NODE (OK)
void at::semantics::PFwriter::processBodyNode
(at::node::BodyNode * const node, int lvl) 
{        
      // The initial section is visible throughout the function
      TypeValidator *tv = new TypeValidator(os(), _symtab, _nestlvl, &_lvltab);
      node->accept(tv, lvl);
      
      _return_body.push(++_lbl);
      _return_func.push(++_lbl);
      
      _symtab.push();
      _lvltab++;     
      if(node->init() != NULL)
      {
        node->init()->accept(this,lvl);
       
      }
      if(node->section() != NULL)
      {
        // all the same...
        node->section()->accept(this,lvl);
      }
      _return_flag = false;
      _pf.LABEL(mklbl(_return_body.top()));
      if(node->end() != NULL)
      {
        node->end()->accept(this,lvl);
      }
      _return_flag = true;
      _pf.LABEL(mklbl(_return_func.top()));
      _return_body.pop();
      _return_func.pop();
}       
//===========================================================================
//---------      A T - S P E C I F I C   I N S T R U C T I O N S ------------
//===========================================================================
// PROCESS PRINT NODE (OK)
void at::semantics::PFwriter::processPrintNode
(at::node::PrintNode * const node, int lvl) 
{
  try 
  {
    TypeValidator *tv =  new TypeValidator(os(), _symtab, _nestlvl, &_lvltab);
    node->accept(tv, lvl);
  }
  catch (std::string s) 
  {
    std::cerr << "FATAL ERROR at " << node->lineno() << " -> " << s << std::endl;
    return;
  }
  node->argument()->accept(this,lvl);
  if(node->argument()->type()->name() == ExpressionType::TYPE_INT)
  {
        _pf.CALL("printi"); // call to the print function INTEGER
        if(node->newline())
        {
           _pf.CALL("println"); // print a newline
        }
        _pf.TRASH(4); // delete the printed value
  }
  if(node->argument()->type()->name() == ExpressionType::TYPE_DOUBLE)
  {
        _pf.CALL("printd"); // call to the print function DOUBLE
        if(node->newline())
        {
           _pf.CALL("println"); // print a newline
        }
        _pf.TRASH(8); // delete the printed value
  }
  if(node->argument()->type()->name() == ExpressionType::TYPE_STRING)
  { 
        _pf.CALL("prints"); // call the print rotine
        if(node->newline())
        {
           _pf.CALL("println"); // print a newline
        }
        _pf.TRASH(4); // remove the printed value
  }
}
//---------------------------------------------------------------------------
// PROCESS FOR NODE 
void at::semantics::PFwriter::processForNode
(at::node::ForNode * const node, int lvl) 
{
  int lbl1;
  int lbl2 = ++_lbl;
  int lbl3 = ++_lbl;
  TypeValidator* tv = new TypeValidator(os(), _symtab, _nestlvl, &_lvltab);
  //PUSH -> Context for Stop and Next instruction
  _contextSTOP.push(lbl2= ++_lbl);
  _contextNEXT.push(lbl3= ++_lbl);
  try 
  {
    if(node->type() == 1)
    {
        node->init()->accept(tv, lvl);
        node->init()->accept(this, lvl);
    }
    else
    {
       node->init()->accept(this, lvl);
       node->init()->accept(tv, lvl);
    }
    _pf.LABEL(mklbl(lbl1 = ++_lbl));
    node->condition()->accept(tv, lvl);
    node->condition()->accept(this, lvl);
    _pf.JZ(mklbl(lbl2));
    node->block()->accept(this, lvl + 2);
    _pf.LABEL(mklbl(lbl3));
    node->incrmt()->accept(tv, lvl);
    node->incrmt()->accept(this, lvl);
    if(node->incrmt()->type()->name() == ExpressionType::TYPE_DOUBLE)
        _pf.TRASH(8);
        else
        _pf.TRASH(4);
    _pf.JMP(mklbl(lbl1));
    _pf.LABEL(mklbl(lbl2));
    
  }
  catch (std::string s) 
  {
    std::cerr << "FATAL ERROR at " << node->lineno() << " -> " << s << std::endl;
    return;
  }
   //________  
   // POP -> Context for Stop and Next instruction
  _contextNEXT.pop();
  _contextSTOP.pop();
}
//---------------------------------------------------------------------------
// PROCESS IF NODE (OK)
void at::semantics::PFwriter::processIfNode
(at::node::IfNode * const node, int lvl) 
{
  int lbl1;
  try 
  {
    TypeValidator *tv = new TypeValidator(os(), _symtab, _nestlvl, &_lvltab);
    node->accept(tv, lvl);
    node->condition()->accept(this, lvl);
    _pf.JZ(mklbl(lbl1 = ++_lbl));
    node->block()->accept(this, lvl + 2);
    _pf.LABEL(mklbl(lbl1));
  }
  catch (std::string s) 
  {
    std::cerr << "FATAL ERROR at " << node->lineno() << " -> " << s << std::endl;
    return;
  }
}
//---------------------------------------------------------------------------
// PROCESS INCLUSIVE NODE (OK)
void at::semantics::PFwriter::processInclusiveNode
(at::node::InclusiveNode * const node, int lvl) 
{
  int lbl1;
  try 
  {
    TypeValidator *tv = new TypeValidator(os(), _symtab, _nestlvl, &_lvltab);
    node->accept(tv, lvl);
    node->condition()->accept(this, lvl);
    _pf.JZ(mklbl(lbl1=++_lbl));
    node->thenblock()->accept(this, lvl + 2);
    _pf.LABEL(mklbl(lbl1));
    node->elseblock()->accept(this, lvl + 2);
  }
  catch (std::string s) 
  {
    std::cerr << "FATAL ERROR at " << node->lineno() << " -> " << s << std::endl;
    return;
  }
}
//---------------------------------------------------------------------------
// PROCESS IF ELSE NODE 
void at::semantics::PFwriter::processIfElseNode
(at::node::IfElseNode * const node, int lvl) 
{
  int lbl1, lbl2;
  try 
  {
        TypeValidator* tv = new TypeValidator(os(), _symtab, _nestlvl, &_lvltab);
        node->accept(tv, lvl);

        node->condition()->accept(this, lvl);
        _pf.JZ(mklbl(lbl1 = ++_lbl));
        node->thenblock()->accept(this, lvl + 2);
        _pf.JMP(mklbl(lbl2 = ++_lbl));
        _pf.LABEL(mklbl(lbl1));
        node->elseblock()->accept(this, lvl + 2);
        _pf.LABEL(mklbl(lbl2));
  }
  catch (std::string s) 
  {
    std::cerr << "FATAL ERROR at " << node->lineno() << " -> " << s << std::endl;
    return;
  }
}
//---------------------------------------------------------------------------
// PROCESS TRASH NODE (OK)
void at::semantics::PFwriter::processTrashNode
(at::node::TrashNode * const node,int lvl) 
{
  try 
  {
        TypeValidator* tv = new TypeValidator(os(), _symtab, _nestlvl, &_lvltab);
        node->accept(tv, lvl);
        
        node->expression()->accept(this, lvl);
        if(node->expression()->type()->name() == ExpressionType::TYPE_DOUBLE)
            _pf.TRASH(8);
        else 
        {
            if(node->expression()->type()->name() == ExpressionType::TYPE_POINTER)
            { 
                PointerType *pt = (PointerType*) node->expression()->type();
                if(pt->PointerName() == ExpressionType::TYPE_DOUBLE)
                {
                   _pf.TRASH(8);
                }
                else
                _pf.TRASH(4);
            }
            else if(node->expression()->type()->name() == ExpressionType::TYPE_VOID);
            else
            _pf.TRASH(4);
        }
  }
  catch (std::string s) 
  {
    std::cerr << "FATAL ERROR at " << node->lineno() << " -> " << s << std::endl;
    return;
  }
}
//---------------------------------------------------------------------------
// PROCESS NEXT NODE (OK)
void at::semantics::PFwriter::processNextNode
(at::node::NextNode *const node, int lvl) 
{
    _pf.JMP(mklbl(_contextNEXT.top()));
}
//---------------------------------------------------------------------------
// PROCESS STOP NODE (OK)
void at::semantics::PFwriter::processStopNode
(at::node::StopNode *const node, int lvl) 
{ 
    _pf.JMP(mklbl(_contextSTOP.top()));
}
//---------------------------------------------------------------------------
// PROCESS RETURN NODE (OK)
void at::semantics::PFwriter::processReturnNode
(at::node::ReturnNode *const node, int lvl) 
{
    if(!_return_flag)
        _pf.JMP(mklbl(_return_func.top()));
    else
       _pf.JMP(mklbl(_return_body.top()));
}
//---------------------------------------------------------------------------
//                               T H E    E N D
//---------------------------------------------------------------------------
