#ifndef __AT_SECTION_NODE_H__
#define __AT_SECTION_NODE_H__

#include <cdk/nodes/Node.h>
#include <cdk/nodes/expressions/Expression.h>

namespace at {
  namespace node {

    class SectionNode: public cdk::node::Node 
    {
        // INIT SEC || FINAL SEC
        int _qualifier;
        cdk::node::Node* _item;
    public:
      inline SectionNode(int lineno, int qualifier, cdk::node::Node *item):
        cdk::node::Node(lineno), _qualifier(qualifier), _item(item){}
        
    public:
      const char *name() 
      {
        return "SectionNode";
      }
      inline cdk::node::Node* item()
      {
        return _item;
      }
      inline int qualifier()
      {
        return _qualifier;
      }
      void accept(SemanticProcessor *sp, int level) {
        sp->processSectionNode(this, level);
      }
    }; // class

  } // namespace node
} // namespace at

#endif  
