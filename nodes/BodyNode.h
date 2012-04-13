#ifndef __AT_BODY_NODE_H__
#define __AT_BODY_NODE_H__

#include <cdk/nodes/Node.h>

namespace at {
  namespace node {
    class BodyNode: public cdk::node::Node 
    {
      cdk::node::Node *_init;
      cdk::node::Node *_section;
      cdk::node::Node *_end;
   
    public:
      inline BodyNode(int lineno, cdk::node::Node *init, cdk::node::Node *section,cdk::node::Node *end) :
        cdk::node::Node(lineno), _init(init), _section(section), _end(end) {}

    public:
      inline cdk::node::Node *init() 
      {
        return _init;
      }
      inline cdk::node::Node *section() 
      {
        return _section;
      }
      inline cdk::node::Node *end() 
      {
        return _end;
      }

    public:
      const char *name() 
      {
        return "at::node::BodyNode";
      }

      void accept(SemanticProcessor *sp, int level) 
      {
        sp->processBodyNode(this, level);
      }
    }; // class
  } // namespace node
} // namespace at
#endif 
