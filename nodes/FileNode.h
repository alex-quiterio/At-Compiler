#ifndef __AT_FILE_NODE_H__
#define __AT_FILE_NODE_H__

#include <cdk/nodes/Node.h>

namespace at {
  namespace node {

    class FileNode: public cdk::node::Node 
    {
      cdk::node::Node *_statements;

    public:
      inline FileNode(int lineno, cdk::node::Node *statements) :
        cdk::node::Node(lineno), _statements(statements) {}

    public:
      inline cdk::node::Node *statements() 
      {
        return _statements;
      }

    public:
      const char *name() const 
      {
        return "FileNode";
      }

      void accept(SemanticProcessor *sp, int level) 
      {
        sp->processFileNode(this, level);
      }
    };
  } // namespace node
} // namespace at
#endif
