#ifndef __AT_SEMANTICS_SYMBOL_H__
#define __AT_SEMANTICS_SYMBOL_H__

#include <string>
#include <cdk/semantics/ExpressionType.h>

class Symbol 
{

  ExpressionType * _symb_info;
  std::string _name;
  int _value;
  // 0 = variable || 1 = Function 
  int _qualifier;

  public:
      inline Symbol(ExpressionType *type, const std::string &name, int value, int qualifier) :
        _symb_info(type), _name(name), _value(value), _qualifier(qualifier) {}

      inline Symbol(ExpressionType *type, const char *name, int value, int qualifier) :
        _symb_info(type), _name(name), _value(value), _qualifier(qualifier){}

      virtual ~Symbol() {}

      inline size_t type() const 
      {
        return _symb_info->name();
      }
      
      inline int size() const 
      {
        return _symb_info->size();
      }
      inline ExpressionType* expression_type()
      {
        return _symb_info;
      }
      inline const std::string &name() const 
      {
        return _name;
      }
      
      inline int value() const 
      {
        return _value;
      }
      
      inline int qualifier() const 
      {
        return _qualifier;
      }
      
      inline int value(int v) 
      {
        return _value = v;
      }
};

#endif
