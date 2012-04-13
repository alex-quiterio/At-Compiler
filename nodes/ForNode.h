#ifndef __AT_FOR_NODE_H__
#define __AT_FOR_NODE_H__

#include <cdk/nodes/Node.h>
#include <cdk/nodes/expressions/Expression.h>

namespace at {
  namespace node {
    class ForNode: public cdk::node::Node 
    {
      int _type;
      cdk::node::Node *_init;
      cdk::node::expression::Expression *_condition;
      cdk::node::expression::Expression *_incrmt;
      cdk::node::Node *_block;

    public:
      inline ForNode(int lineno,int type, cdk::node::Node *init, cdk::node::expression::Expression *condition, cdk::node::expression::Expression *inc,cdk::node::Node *block) :
        cdk::node::Node(lineno),_type(type), _init(init), _condition(condition), _incrmt(inc), _block(block) {
      }

    public:
      inline int type() 
      {
        return _type;
      }

      inline cdk::node::Node *init() 
      {
        return _init;
      }
      inline cdk::node::expression::Expression *condition() 
      {
        return _condition;
      }
      inline cdk::node::expression::Expression *incrmt() 
      {
        return _incrmt;
      }
      inline cdk::node::Node *block() 
      {
        return _block;
      }

    public:
      const char *name() 
      {
        return "ForNode";
      }

      void accept(SemanticProcessor *sp, int level) 
      {
        sp->processForNode(this, level);
      }
    }; // class
  } // namespace node
} // namespace at
#endif
