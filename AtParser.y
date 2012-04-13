%{
#include <cstdlib>  // needed for getenv
#include <iostream>
#include <string>
#include <sstream>

#include "syntax/FlexBYACC.h"
#include "nodes/all.h"
#include "semantics/PointerType.h"

#define LINE scanner()->lineno()
#define yyparse at::syntax::FlexBYACC::yyparse
%}

%union {

  int                                i;		/* integer value */
  float				     r; 	/* real value */
  std::string                       *s;		/* symbol name or string literal */
  cdk::node::Node                   *node;	/* node pointer */
  cdk::node::Sequence               *sequence;
  cdk::node::expression::Expression *expression;/* expression nodes */
  cdk::node::expression::String	    *simpleString;
  ExpressionType		    *expressiontype;
  at::node::expression::NestedVar     *nestvar;
};

%token <i> tINTEGER
%token <s> tIDENTIFIER tSTRING tCHAR tTEXT
%token <r> tREAL
%token tPRINT tRETURN tFLOAT tFUNCTION tOPENBODY
%token tSTOP tNEXT tUSE tPUBLIC tSTRING tCLOSEBODY
%token tINT tVOID tCONST
%nonassoc tIFX
%nonassoc '[' '{' '(' 
%nonassoc ':' 


%left tGE tLE tEQ tNE '>' '<' tOR
%left tAND
%right '=' 
%left '+' '-'
%left '*' '/' '%'
%right '~'
%right'^'
%nonassoc tUMINUS '#'

%type <node> instr program cond_i iter_i sectype body  section
%type <node> var declr func funcno file_d cond_ii instr_rtr
%type <node> ipa arg end init block_s func_glb var_glb
%type <sequence> block listblck vars listdecl file
%type <sequence> exprs in_args
%type <expression> literal zexpr expr invoc index index_1
%type <expression> ids left_v
%type <nestvar> at
%type <simpleString> strbody string
%type <expressiontype> type

%%

program	: file
        {
          _compiler->syntax(new at::node::FileNode(LINE, $1));
        }
	;
//===========================================================================
//---------                    AT - FILE NODE                    ------------
//===========================================================================
file	: file_d	{$$ = new cdk::node::Sequence(LINE,$1);}
	| file file_d	{$$ = new cdk::node::Sequence(LINE,$2,$1);}
	;
file_d	: var_glb ';'	{$$ = $1;}
	| func_glb	{$$ = $1;}
	;
//===========================================================================
//---------                     AT - DECLARATIONS                ------------
//===========================================================================
declr	: var';'	{$$ =$1;}
	| func';'	{$$ =$1;}
	| funcno';'	{$$ =$1;}
	;
listdecl: declr 		{$$ = new cdk::node::Sequence(LINE, $1);}
	| listdecl  declr 	{$$ = new cdk::node::Sequence(LINE,$2, $1);}
	;
//===========================================================================
//---------                   AT - FUNCTIONS   (OK)              ------------
//===========================================================================
func_glb: tPUBLIC type tIDENTIFIER ipa body 			{$$ = new at::node::FunctionNode(LINE,tPUBLIC,$2,new cdk::node::expression::Identifier(LINE, $3), $4,NULL,$5);}
	| tPUBLIC type tIDENTIFIER ipa tFUNCTION literal body	{$$ = new at::node::FunctionNode(LINE,tPUBLIC,$2,new cdk::node::expression::Identifier(LINE, $3),$4,$6,$7);}
	| tPUBLIC type tIDENTIFIER ipa	 			{$$ = new at::node::FunctionDeclNode(LINE,tPUBLIC,$2,new cdk::node::expression::Identifier(LINE, $3), $4,NULL);}
	| tPUBLIC type tIDENTIFIER ipa tFUNCTION literal 	{$$ = new at::node::FunctionDeclNode(LINE,tPUBLIC,$2,new cdk::node::expression::Identifier(LINE, $3), $4,$6);}
	| tUSE type tIDENTIFIER ipa 				{$$ = new at::node::FunctionDeclNode(LINE,tUSE,$2,new cdk::node::expression::Identifier(LINE, $3), $4,NULL);}
	| tUSE type tIDENTIFIER ipa tFUNCTION literal		{$$ = new at::node::FunctionDeclNode(LINE,tUSE,$2,new cdk::node::expression::Identifier(LINE, $3), $4,$6);}
	| func							{$$ = $1;}
	| funcno						{$$ = $1;}
	;
func	: type tIDENTIFIER ipa body 				{$$ = new at::node::FunctionNode(LINE,0,$1,new cdk::node::expression::Identifier(LINE, $2),$3,NULL,$4);}
	| type tIDENTIFIER ipa tFUNCTION literal body		{$$ = new at::node::FunctionNode(LINE,0,$1,new cdk::node::expression::Identifier(LINE, $2),$3,$5,$6);}
	;
funcno	: type tIDENTIFIER ipa	 				{$$ = new at::node::FunctionDeclNode(LINE,0,$1,new cdk::node::expression::Identifier(LINE, $2), $3,NULL);}
	| type tIDENTIFIER ipa tFUNCTION literal 		{$$ = new at::node::FunctionDeclNode(LINE,0,$1,new cdk::node::expression::Identifier(LINE, $2),$3,$5);}
	;
//===========================================================================
//---------                   AT - FUNCTION ARGUMENTS            ------------
//===========================================================================
ipa	: '('')'			{$$ = new cdk::node::Nil(LINE);}
	| '('in_args')'			{$$ = $2;}
	;
in_args	: arg				{ $$ = new cdk::node::Sequence(LINE,$1);}
	| in_args ',' arg		{ $$ = new cdk::node::Sequence(LINE,$3, $1);}
	;
arg	: type tIDENTIFIER		{ $$ = new at::node::InputArgumentNode(LINE,$1,new cdk::node::expression::Identifier(LINE, $2), NULL);}
	| type tIDENTIFIER '=' expr	{ $$ = new at::node::InputArgumentNode(LINE,$1,new cdk::node::expression::Identifier(LINE, $2), $4);}
	;
//===========================================================================
//---------                      AT - INTRUCTIONS (OK)           ------------
//===========================================================================
instr	: ';'                   { $$ = new cdk::node::Nil(LINE); }
 	| expr '!'      	{ $$ = new at::node::PrintNode(LINE, $1,false); }
	| expr ';'		{ $$ = new at::node::TrashNode(LINE, $1);}
	| expr tPRINT		{ $$ = new at::node::PrintNode(LINE, $1,true);  }
	| tNEXT			{ $$ = new at::node::NextNode(LINE);}
	| tSTOP			{ $$ = new at::node::StopNode(LINE);}
	| cond_i		{ $$ = $1;}
	| cond_ii		{ $$ = $1;}
	| iter_i		{ $$ = $1;}
	| block_s		{ $$ = $1;}
	;
//===========================================================================
//---------             AT - SPECIFIC CONDITION NODES (OK)       ------------
//===========================================================================
cond_i	: '['expr']' '#' instr_rtr 	  	{$$ = new at::node::IfNode(LINE,$2,$5);}
	;
cond_ii	: '['expr']' '?' instr_rtr  %prec tIFX	{$$ = new at::node::IfNode(LINE,$2,$5);}
	| '['expr']' '?' instr_rtr  cond_ii	{$$ = new at::node::IfElseNode(LINE,$2,$5,$6);}
	| '['expr']' '?' instr_rtr':'instr_rtr  {$$ = new at::node::IfElseNode(LINE,$2,$5,$7);}
	;
instr_rtr:   instr				{$$ = $1;}
	|    tRETURN 				{$$ = new at::node::ReturnNode(LINE);}
	;
//===========================================================================
//---------              AT - SPECIFIC ITERATION NODES           ------------
//===========================================================================
iter_i	: '['zexpr ';' zexpr ';' zexpr']' instr_rtr {$$ = new at::node::ForNode(LINE,1, $2, $4, $6, $8);}
	| '['vars ';' zexpr ';' zexpr']' instr_rtr {$$ = new at::node::ForNode(LINE,0, $2, $4, $6, $8);}
	;
//===========================================================================
//---------              AT - SPECIFIC BLOCK NODES               ------------
//===========================================================================
block	: '{''}'	    	{ $$ = new cdk::node::Sequence(LINE, new cdk::node::Nil(LINE)); }
	| '{'listblck'}' 	{ $$ = new cdk::node::Sequence(LINE, $2); }
	| '{'listdecl'}' 	{ $$ = new cdk::node::Sequence(LINE, $2); }
	| '{'listdecl listblck'}'{$$ = new cdk::node::Sequence(LINE, $3, $2);}
	| '{'tRETURN'}'	    	{ $$ = new cdk::node::Sequence(LINE, new at::node::ReturnNode(LINE)); }
	| '{'listblck tRETURN'}'{ $$ = new cdk::node::Sequence(LINE, new at::node::ReturnNode(LINE),$2);}
	| '{'listdecl tRETURN'}'{ $$ = new cdk::node::Sequence(LINE, new at::node::ReturnNode(LINE),$2);}
	| '{'listdecl listblck tRETURN'}'{ $$ = new cdk::node::Sequence(LINE,new at::node::ReturnNode(LINE),new cdk::node::Sequence(LINE,$3, $2));}
	;
listblck: instr			{ $$ = new cdk::node::Sequence(LINE, $1);}
	| listblck instr	{ $$ = new cdk::node::Sequence(LINE, $2, $1);}
	;
//===========================================================================
//---------           AT - SPECIFIC VARIABLES NODES              ------------
//===========================================================================
var_glb	: tPUBLIC type tIDENTIFIER '=' expr	{$$ = new at::node::DeclarationNode(LINE,0,0,tPUBLIC,$2,new at::node::expression::LeftValue(LINE, $3),$5);}
	| tCONST tPUBLIC type tIDENTIFIER'='expr{$$ = new at::node::DeclarationNode(LINE,0,tCONST,tPUBLIC,$3,new at::node::expression::LeftValue(LINE, $4),$6);}
	| tCONST tUSE type tIDENTIFIER		{$$ = new at::node::DeclarationNode(LINE,tUSE,tCONST,0,$3,new at::node::expression::LeftValue(LINE, $4),NULL);}
	| tUSE type tIDENTIFIER			{$$ = new at::node::DeclarationNode(LINE,tUSE,0,0,$2,new at::node::expression::LeftValue(LINE, $3),NULL);}
	| tPUBLIC type tIDENTIFIER		{$$ = new at::node::DeclarationNode(LINE,0,0,tPUBLIC,$2,new at::node::expression::LeftValue(LINE, $3),NULL);}
	| var					{$$ = $1;}
	;
var	: type tIDENTIFIER '=' expr		{$$ = new at::node::DeclarationNode(LINE,0,0,0,$1,new at::node::expression::LeftValue(LINE, $2),$4);}
	| tCONST type tIDENTIFIER '=' expr	{$$ = new at::node::DeclarationNode(LINE,0,tCONST,0,$2,new at::node::expression::LeftValue(LINE, $3),$5);}
	| tCONST tPUBLIC type tIDENTIFIER	{$$ = new at::node::DeclarationNode(LINE,0,tCONST,tPUBLIC,$3,new at::node::expression::LeftValue(LINE, $4),NULL);}
	| type tIDENTIFIER			{$$ = new at::node::DeclarationNode(LINE,0,0,0,$1,new at::node::expression::LeftValue(LINE, $2),NULL);}
	| tCONST type tIDENTIFIER		{$$ = new at::node::DeclarationNode(LINE,0,tCONST,0,$2,new at::node::expression::LeftValue(LINE, $3),NULL);}
	;
//===========================================================================
//---------           AT - SPECIFIC VARIABLES NODES (SPECIALS)   ------------
//===========================================================================
vars	: var				{ $$ = new cdk::node::Sequence(LINE,$1);}
	| vars ',' var			{ $$ = new cdk::node::Sequence(LINE,$3, $1);}
	;
//===========================================================================
//---------           AT - SPECIFIC TYPES & LITERALS             ------------
//===========================================================================
type	: tINT		{$$ = new ExpressionType(4,ExpressionType::TYPE_INT);}
	| tFLOAT	{$$ = new ExpressionType(8,ExpressionType::TYPE_DOUBLE);}
	| tSTRING	{$$ = new ExpressionType(4,ExpressionType::TYPE_STRING);}
	| tVOID		{$$ = new ExpressionType(0,ExpressionType::TYPE_VOID);}
	| '<'type'>'	{$$ = new at::semantics::PointerType($2);}
	;
literal	: tINTEGER      { $$ = new cdk::node::expression::Integer(LINE, $1); }
	| tREAL		{ $$ = new cdk::node::expression::Double(LINE, $1);}
	| tSTRING	{ $$ = new cdk::node::expression::String(LINE, $1); }
	;
//===========================================================================
//---------         AT - SPECIFIC SECTION FUNCTIONS              ------------
//===========================================================================
sectype	: '('')'      block_s  %prec tIFX	{$$ = new at::node::IfNode(LINE, new cdk::node::expression::Integer(LINE, 1), $3);}
	| '('')'      block_s  sectype 		{$$ = new at::node::InclusiveNode(LINE,new cdk::node::expression::Integer(LINE, 1), $3, $4);}
	| '['zexpr']' block_s  %prec tIFX	{$$ = new at::node::IfNode(LINE, $2, $4);}
	| '['zexpr']' block_s  sectype		{$$ = new at::node::IfElseNode(LINE, $2, $4, $5);}
	| '('expr')'  block_s  %prec tIFX 	{$$ = new at::node::IfNode(LINE,$2,$4);}
	| '('expr')'  block_s  sectype 		{$$ = new at::node::InclusiveNode(LINE,$2,$4,$5);}
	| 	      block_s  %prec tIFX	{$$ = $1;}
	|	      block_s  sectype		{$$ = new at::node::InclusiveNode(LINE,new cdk::node::expression::Integer(LINE, 1),$1,$2);}
	;
block_s : block				{$$ = new at::node::SectionNode(LINE,0,$1);}
	;
section	: sectype			{$$ = $1;}
	;
//===========================================================================
//---------         AT - SPECIFIC BODY FUNCTIONS 	         ------------
//===========================================================================
body	: init	 			{$$ = new at::node::BodyNode(LINE,$1,NULL,NULL);}
	| section			{$$ = new at::node::BodyNode(LINE,NULL,$1,NULL);}
	| end				{$$ = new at::node::BodyNode(LINE,NULL,NULL,$1);}
	| init section			{$$ = new at::node::BodyNode(LINE,$1,$2,NULL);}
	| init end			{$$ = new at::node::BodyNode(LINE,$1,NULL,$2);}
	| section end			{$$ = new at::node::BodyNode(LINE,NULL,$1,$2);}
	| init section end		{$$ = new at::node::BodyNode(LINE,$1,$2,$3);}
 	;
init	: tOPENBODY block		{$$ = new at::node::SectionNode(LINE,1,$2);}
	;
end	: tCLOSEBODY block		{$$ = new at::node::SectionNode(LINE,0,$2);}
	;
//===========================================================================
//---------     AT - SPECIFIC EXPRESSIONS (SPECIAL EXPRS)	 ------------
//===========================================================================
exprs	: expr				{$$ = new cdk::node::Sequence(LINE,$1);}
	| exprs ',' expr		{ $$ = new cdk::node::Sequence(LINE,$3, $1);}
	;
zexpr	: expr 				{ $$ = $1;}
	| /* zero expression*/  	{ $$ = new cdk::node::expression::Integer(LINE, 1);}
	;
//===========================================================================
//---------     AT - SPECIFIC EXPRESSIONS (INVOC FUNCTIONS)	 ------------
//===========================================================================
invoc	: tIDENTIFIER '('')'		{ $$ = new at::node::expression::CallFunction(LINE,new cdk::node::expression::Identifier(LINE, $1),NULL);}
	| tIDENTIFIER '('exprs')'	{ $$ = new at::node::expression::CallFunction(LINE,new cdk::node::expression::Identifier(LINE, $1), $3);}
	| '@''('')'			{ $$ = new at::node::expression::CallFunction(LINE,new cdk::node::expression::Identifier(LINE,"@"),NULL);}
	| '@''('exprs')'		{ $$ = new at::node::expression::CallFunction(LINE,new cdk::node::expression::Identifier(LINE,"@"), $3);}
	;
//===========================================================================
//---------     AT - SPECIFIC EXPRESSIONS (INDEX POINTERS)	 ------------
//===========================================================================
index 	: tIDENTIFIER   index_1		{ $$ = new at::node::expression::AccessPointer(LINE,new cdk::node::expression::Identifier(LINE, $1),$2);}
	;
index_1	:'[' expr ']'			{ $$ =new at::node::expression::AccessPointer(LINE,$2);}
	|'[' expr ']'	index_1		{ $$ = new at::node::expression::AccessPointer(LINE,$2, $4);}
	;
at	: '@''.' tIDENTIFIER		{$$ = new at::node::expression::NestedVar(LINE, new cdk::node::expression::Identifier(LINE, $3));}
	| '@''.' at			{$$ = new at::node::expression::NestedVar(LINE,$3);}
	| '@''.''@'			{$$ = new at::node::expression::NestedVar(LINE, new cdk::node::expression::Identifier(LINE, "@"));}
	;
left_v	: tIDENTIFIER           	{$$ = new at::node::expression::LeftValue(LINE, $1);}
	| at				{$$ = $1;}
	| index				{$$ = $1;}
	| '@'				{$$ = new at::node::expression::LeftValue(LINE, "@");}
	;
ids	: tIDENTIFIER           	{$$ = new cdk::node::expression::Identifier(LINE, $1);}
	| at				{$$ = new at::node::expression::Position(LINE,0,$1);}
	| index				{$$ = new at::node::expression::Position(LINE,1,$1);}
	;
//===========================================================================
//---------               AT - SPECIFIC EXPRESSIONS              ------------
//===========================================================================
expr	: string		{$$ = $1;}
	| tREAL			{$$ = new cdk::node::expression::Double(LINE, $1);}
	| tINTEGER              {$$ = new cdk::node::expression::Integer(LINE, $1); }
	| ids			{$$ = $1;}
	| left_v '?'		{$$ = new at::node::expression::QuestionMark(LINE,$1);}
	| left_v '=' expr  	{$$ = new at::node::expression::AtribNode(LINE,$1,$3);}
	| '@'			{$$ = new at::node::expression::ReadNode(LINE);}
	| invoc			{$$ = $1;}
	|'(' expr ')'		{$$ = $2;}
	|'#' tINTEGER		{$$ = new at::node::expression::ALLOC(LINE,new cdk::node::expression::Integer(LINE, $2));}
	|'+' expr %prec tUMINUS {$$ = $2;}
	| expr '+' expr  	{$$ = new cdk::node::expression::ADD(LINE, $1, $3);}
	| expr '-' expr 	{$$ = new cdk::node::expression::SUB(LINE, $1, $3);}
        | expr '*' expr 	{$$ = new cdk::node::expression::MUL(LINE, $1, $3);}
	| expr '/' expr		{$$ = new cdk::node::expression::DIV(LINE, $1, $3);}
	| expr '<' expr  	{$$ = new cdk::node::expression::LT(LINE, $1, $3);}
	| expr '>' expr 	{$$ = new cdk::node::expression::GT(LINE, $1, $3);}
	| expr tGE expr  	{$$ = new cdk::node::expression::GE(LINE, $1, $3);}
	| expr tLE expr 	{$$ = new cdk::node::expression::LE(LINE, $1, $3);}
	| expr tNE expr  	{$$ = new cdk::node::expression::NE(LINE, $1, $3);}
	| expr tEQ expr  	{$$ = new cdk::node::expression::EQ(LINE, $1, $3);}
	| '~' expr		{$$ = new at::node::expression::NOT(LINE, $2);}
	| expr '%' expr  	{$$ = new cdk::node::expression::MOD(LINE, $1, $3);}
	| expr '^' expr		{$$ = new at::node::expression::POWER(LINE,$1,$3);}
	| expr tOR expr  	{$$ = new at::node::expression::OR(LINE, $1, $3); }
	| expr tAND expr	{$$ = new at::node::expression::AND(LINE, $1, $3);}
	| '-' expr %prec tUMINUS{$$ = new cdk::node::expression::NEG(LINE, $2);}
	;
//===========================================================================
//---------            AT - SPECIFIC STRING CONFIG (OK)          ------------
//===========================================================================
string	:  tTEXT		{ $$ = new cdk::node::expression::String(LINE, $1); }
	 | tTEXT strbody	{ $$ = new cdk::node::expression::String(LINE, *$1 + $2->value() ); }
	 | tINTEGER strbody	{ $$ = new cdk::node::expression::String(LINE, std::string(1,$1) + $2->value() ); }
	 | tCHAR strbody	{ $$ = new cdk::node::expression::String(LINE, *$1 + $2->value() ); }
	 ;
strbody	: tINTEGER		{ $$ = new cdk::node::expression::String(LINE, std::string(1,$1)); }
	 | tCHAR		{ $$ = new cdk::node::expression::String(LINE, $1); }
	 | tTEXT		{ $$ = new cdk::node::expression::String(LINE, $1); }
	 | strbody tINTEGER	{ $$ = new cdk::node::expression::String(LINE, $1->value() + std::string(1,$2)); }
	 | strbody tCHAR	{ $$ = new cdk::node::expression::String(LINE, $1->value() + *$2); }
	 | strbody tTEXT	{ $$ = new cdk::node::expression::String(LINE, $1->value() + *$2); }
	 ;
%%