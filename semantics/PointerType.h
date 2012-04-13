#ifndef __AT_SEMANTICS_POINTER_TYPE_H__
#define __AT_SEMANTICS_POINTER_TYPE_H__

#include <cdk/semantics/ExpressionType.h>

namespace at {
    namespace semantics {
        class PointerType : public ExpressionType {
        
        int _nestinglvl;
        size_t _name;
        
        public:
        inline PointerType(ExpressionType* ET): 
        ExpressionType(4,ExpressionType::TYPE_POINTER),_nestinglvl(0),_name(0)
        {
            if(ET->name() == ExpressionType::TYPE_POINTER)
            {
                PointerType* pointer = (PointerType*) ET;
                _nestinglvl = pointer->NestLvl() + 1;
                _name  = pointer->PointerName();
                delete pointer;
            }
            else
            {
                _nestinglvl +=1;
                _name = ET->name();
                delete ET;
            }
       }
       public:
       inline PointerType(int nest, size_t name): 
       ExpressionType(4,ExpressionType::TYPE_POINTER),
       _nestinglvl(nest),_name(name){}
       
       inline int NestLvl()
       {
            return _nestinglvl;
       } 
       inline size_t PointerName()
       {
            return _name;
       }
    };
  }
}
        
#endif
