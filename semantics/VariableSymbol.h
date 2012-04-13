#ifndef __AT_SEMANTICS_VAR_SYMBOL_H__
#define __AT_SEMANTICS_VAR_SYMBOL_H__

#include <string>
#include <cdk/semantics/ExpressionType.h>

#include "semantics/Symbol.h"
/*
 * Atributos: tipo da var, id, valor, tamanho, offset;
 * se o offset for zero a variavel é global
 */
class VariableSymbol : public Symbol
{

private:
  int _offset;
  int _nestlvl;
  
public:
  inline VariableSymbol(ExpressionType *type, const std::string &name, int offset, int nest) :
    Symbol(type, name, 0,0), _offset(offset), _nestlvl(nest){}
    
public:
  inline VariableSymbol(ExpressionType *type, const char *name, int offset, int nest) :
    Symbol(type, name, 0,0),_offset(offset), _nestlvl(nest){}

  virtual ~VariableSymbol() {}
  
  public:
  inline int OffSet()
  {
        return _offset;
  }
  inline size_t NestLvl()
  {
        return _nestlvl;
  }
};
#endif  
