//============================================================================
//---------      A T ->  A R G U M E N T   C O U N T E R          ------------
//============================================================================
#include <cdk/semantics/ExpressionType.h>

#include "semantics/ArgumentCounter.h"
#include "nodes/all.h"  
//===========================================================================
// DECLARATION NODE
void at::semantics::ArgumentCounter::processDeclarationNode
(at::node::DeclarationNode *const node, int lvl) 
{
            if(node->qualifier() == 0)
            _args += node->type()->size();
}
//===========================================================================
// INPUT ARGUMENT NODE
void at::semantics::ArgumentCounter::processInputArgumentNode
(at::node::InputArgumentNode *const node, int lvl) 
{
                _args += node->type()->size();
}
//===========================================================================
// FUNCTION NODE
void at::semantics::ArgumentCounter::processFunctionNode
(at::node::FunctionNode *const node, int lvl) 
{
            node->body()->accept(this, lvl);
}
//===========================================================================
// FUNCTION BODY NODE
void at::semantics::ArgumentCounter::processBodyNode
(at::node::BodyNode *const node, int lvl) 
{
      if(node->init() != NULL)
      node->init()->accept(this,lvl);
      if(node->section() != NULL)
      node->section()->accept(this,lvl);
      if(node->end() != NULL)
      node->end()->accept(this,lvl);
}
//===========================================================================
// FOR NODE
void at::semantics::ArgumentCounter::processForNode
(at::node::ForNode *const node, int lvl) 
{
      node->init()->accept(this, lvl);
      node->block()->accept(this,lvl);
}
//===========================================================================
// FUNCTION SEQUENCE 
void at::semantics::ArgumentCounter::processSequence
(cdk::node::Sequence *const node, int lvl) 
{
  for (size_t i = 0; i < node->size(); i++) 
  {
    node->node(i)->accept(this, lvl);
  }
}
//===========================================================================
// FUNCTION SECTION
void at::semantics::ArgumentCounter::processSectionNode
(at::node::SectionNode *const node, int lvl) 
{
  
  node->item()->accept(this, lvl);
}
//---------------------------------------------------------------------------
// IF NODE (OK)
void at::semantics::ArgumentCounter::processIfNode
(at::node::IfNode *const node, int lvl) 
{
    node->condition()->accept(this, lvl+2);
    node->block()->accept(this, lvl+2);    
}
//---------------------------------------------------------------------------
// IF ELSE NODE (OK)
void at::semantics::ArgumentCounter::processIfElseNode
(at::node::IfElseNode *const node, int lvl) 
{
    node->condition()->accept(this, lvl+2);
    node->thenblock()->accept(this, lvl+2);    
    node->elseblock()->accept(this, lvl+2);
}
//---------------------------------------------------------------------------
// Inclusive NODE (OK)
void at::semantics::ArgumentCounter::processInclusiveNode
(at::node::InclusiveNode *const node, int lvl) 
{
    node->condition()->accept(this, lvl+2);
    node->thenblock()->accept(this, lvl+2);    
    node->elseblock()->accept(this, lvl+2);
}
//---------------------------------------------------------------------------
//                                  T H E    E N D
//---------------------------------------------------------------------------
  
