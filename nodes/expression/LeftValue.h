#ifndef __AT_NODE_EXPRESSION_LEFTVALUE_H__
#define __AT_NODE_EXPRESSION_LEFTVALUE_H__

#include <cdk/nodes/expressions/Simple.h>
#include <string>
#include "SemanticProcessor.h"

namespace at {
  namespace node {
    namespace expression {

      /**
       * Class for describing syntactic tree leaves for holding lvalues.
       */
      class LeftValue: public cdk::node::expression::Simple<std::string> {
      public:
        inline LeftValue(int lineno, const char *s) :
          cdk::node::expression::Simple<std::string>(lineno, s) {
        }
        inline LeftValue(int lineno, const std::string &s) :
          cdk::node::expression::Simple<std::string>(lineno, s) {
        }
        inline LeftValue(int lineno, const std::string *s) :
          cdk::node::expression::Simple<std::string>(lineno, *s) {
        }

      public:

        /**
         * @return the name of the node's class
         */
        const char *name() const {
          return "at::node::expression::LeftValues";
        }

        /**
         * @param sp semantic processor visitor
         * @param level syntactic tree level
         */
        virtual void accept(SemanticProcessor *sp, int level) {
          sp->processLeftValue(this, level);
        }

      };

    } // expression
  } // node
} // at

#endif
