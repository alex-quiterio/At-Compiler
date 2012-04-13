#ifndef __AT_SEMANTICS_FUNCT_SYMBOL_H__
#define __AT_SEMANTICS_FUNCT_SYMBOL_H__

#include <string>
#include <vector>
#include <cdk/semantics/ExpressionType.h>
#include <cdk/nodes/expressions/Expression.h>
#include "semantics/Symbol.h"

/* 
 * Atributos: type, return value, id, size of input arguments.
 */
class FunctionSymbol : public Symbol
{
    int  _size_args;
    std::string _nasm_name;
    bool _written;
    std::vector<cdk::node::expression::Expression*> _literals;
    std::vector<ExpressionType*> _types;
    
public:
  inline FunctionSymbol(ExpressionType *type, const std::string &name, bool writen = true) :
    Symbol(type, name, 0,1), _written(writen), _literals(), _types(){}
    
public:
  inline FunctionSymbol(ExpressionType *type, const char *name, bool writen=true) :
    Symbol(type, name, 0,1), _written(writen), _literals(), _types(){}
    
public:
  inline void literal_push(cdk::node::expression::Expression* expr)
  {
    _literals.push_back(expr);
  }
  inline void type_push(ExpressionType* expr)
  {
    _types.push_back(expr);
  }
public:
  inline size_t types_size()
  {
    return _types.size();
  }
  inline size_t literal_size()
  {
    return _literals.size();
  }
public:
  inline ExpressionType* types_at(int index)
  {
    return _types.at(index);
  }
  inline cdk::node::expression::Expression* lit_at(int index)
  {
    return _literals.at(index);
  }
public:
  inline void SetNasmName(std::string name)
  {
    _nasm_name = name;
  }
  inline std::string NasmName()
  { 
    return _nasm_name;
  }
public:
  inline void SetSizeArgs(int size)
  {
    _size_args = size;
  }
  inline bool written()
  {
    return _written;
  }
public:
  inline void written(bool state)
  {
    _written = state;
  }
  inline int SizeOfArgs()
  {
    return _size_args;
  }
  virtual ~FunctionSymbol() {}
};
#endif 
