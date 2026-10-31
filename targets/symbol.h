#ifndef __AT_TARGETS_SYMBOL_H__
#define __AT_TARGETS_SYMBOL_H__

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <cdk/types/basic_type.h>

// not <cdk/ast/expression_node.h>: CDK's node headers include
// targets/basic_ast_visitor.h, which includes this file
namespace cdk { class expression_node; }

namespace at {

  //! A variable or a function.
  class symbol {
    std::string _name;
    std::shared_ptr<cdk::basic_type> _type;  // for functions: the return type
    bool _function;
    int _depth;          // nesting level of the owning function; 0 is global
    int _offset = 0;     // frame offset (locals < 0, arguments >= 8); globals use the label
    std::string _label;  // assembly name of globals and functions
    bool _defined = false;
    bool _external = false;

  public:
    // functions only
    std::vector<std::shared_ptr<cdk::basic_type>> arguments;
    std::vector<cdk::expression_node*> defaults;  // per argument, nullptr if none

    symbol(const std::string &name, std::shared_ptr<cdk::basic_type> type, bool function, int depth) :
        _name(name), _type(type), _function(function), _depth(depth) {
    }

    const std::string &name() const { return _name; }
    std::shared_ptr<cdk::basic_type> type() const { return _type; }
    bool is_typed(cdk::typename_type name) const { return _type->name() == name; }
    bool function() const { return _function; }
    int depth() const { return _depth; }
    bool global() const { return _depth == 0; }

    int offset() const { return _offset; }
    void offset(int offset) { _offset = offset; }

    const std::string &label() const { return _label; }
    void label(const std::string &label) { _label = label; }

    bool defined() const { return _defined; }
    void defined(bool defined) { _defined = defined; }

    //! declared with "use" and never defined here
    bool external() const { return _external; }
    void external(bool external) { _external = external; }

    int arguments_size() const {
      int size = 0;
      for (auto &t : arguments) size += t->size();
      return size;
    }
  };

  //! Nested scopes. Unlike cdk::symbol_table, a lookup can be limited to
  //! the scopes of an enclosing function, which "@.x" needs.
  class scopes {
    std::vector<std::map<std::string, std::shared_ptr<symbol>>> _contexts;
    std::vector<std::shared_ptr<symbol>> _functions;  // functions being compiled, innermost last

  public:
    scopes() : _contexts(1) {}

    void push() { _contexts.emplace_back(); }
    void pop() { _contexts.pop_back(); }

    bool insert(std::shared_ptr<symbol> s) {
      return _contexts.back().emplace(s->name(), s).second;
    }

    std::shared_ptr<symbol> find_local(const std::string &name) const {
      auto it = _contexts.back().find(name);
      return it == _contexts.back().end() ? nullptr : it->second;
    }

    //! innermost symbol with this name owned by a function at most `depth` deep
    std::shared_ptr<symbol> find(const std::string &name, int depth = 1 << 30) const {
      for (auto ctx = _contexts.rbegin(); ctx != _contexts.rend(); ++ctx) {
        auto it = ctx->find(name);
        if (it != ctx->end() && it->second->depth() <= depth) return it->second;
      }
      return nullptr;
    }

    void enter_function(std::shared_ptr<symbol> f) { _functions.push_back(f); }
    void leave_function() { _functions.pop_back(); }
    std::shared_ptr<symbol> function() const { return _functions.empty() ? nullptr : _functions.back(); }
    int depth() const { return (int)_functions.size(); }
  };

} // at

#endif
