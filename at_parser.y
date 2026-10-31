%{
//-- don't change *any* of these: if you do, you'll break the compiler.
#include <algorithm>
#include <memory>
#include <cstring>
#include <cdk/compiler.h>
#include <cdk/types/types.h>
#include ".auto/all_nodes.h"
#define LINE                  compiler->scanner()->lineno()
#define yylex()               compiler->scanner()->scan()
#define yyerror(compiler, s)  compiler->scanner()->error(s)
//-- don't change *any* of these --- END!

#define EMPTY (new cdk::sequence_node(LINE))

// Hang `next` on the else branch of the open "[c] ? i" ladder ending in
// `prev`, if both are "?" conditionals. Returns false when they don't chain.
static bool chain_conditional(cdk::basic_node *prev, cdk::basic_node *next) {
  auto q = dynamic_cast<at::if_node*>(next);
  if (!(q && q->chainable()) && !dynamic_cast<at::if_else_node*>(next)) return false;
  for (auto cur = dynamic_cast<at::if_node*>(prev); cur && cur->chainable();
       cur = dynamic_cast<at::if_node*>(cur->elseblock())) {
    if (cur->elseblock() == nullptr) {
      cur->elseblock(next);
      return true;
    }
  }
  return false;
}
%}

%parse-param {std::shared_ptr<cdk::compiler> compiler}

%union {
  //--- don't change *any* of these: if you do, you'll break the compiler.
  YYSTYPE() : type(cdk::primitive_type::create(0, cdk::TYPE_VOID)) {}
  ~YYSTYPE() {}
  YYSTYPE(const YYSTYPE &other) { *this = other; }
  YYSTYPE& operator=(const YYSTYPE &other) { type = other.type; return *this; }

  std::shared_ptr<cdk::basic_type> type;        /* expression type */
  //-- don't change *any* of these --- END!

  int                   i;    /* integer value */
  double                d;    /* real value */
  std::string          *s;    /* identifier or string literal */
  cdk::basic_node      *node;
  cdk::sequence_node   *sequence;
  cdk::expression_node *expression;
  cdk::lvalue_node     *lvalue;
  at::block_node       *block;
  at::body_node        *body;
  at::nested_variable_node *nested;
}

%token <i> tINTEGER
%token <d> tREAL
%token <s> tIDENTIFIER tTEXT tCHAR
%token tTYPE_INT tTYPE_REAL tTYPE_STRING tTYPE_VOID
%token tPUBLIC tUSE tCONST tFUNCTION tOPENBODY tCLOSEBODY
%token tNEXT tSTOP tRETURN tPRINTLN

/* C-like precedence (the old grammar put "||" with the comparisons and
 * "&&" above them, which gives "10==3 && 10!=3" the wrong value). tIFX
 * settles the dangling ":" of "[c] ? i : i" in favour of shifting. */
%nonassoc tIFX
%nonassoc ':'
%right '='
%left tOR
%left tAND
%left tEQ tNE
%left tGE tLE '>' '<'
%left '+' '-'
%left '*' '/' '%'
%nonassoc tUMINUS '~' '#'
%right '^'

%type <sequence> globals arguments declarations instructions exprs opt_exprs vars sections
%type <node> global function var argument declaration instruction conditional iteration section
%type <expression> literal opt_literal opt_init expr opt_expr string invocation
%type <s> strbody
%type <lvalue> lvalue index
%type <nested> nested
%type <block> block initial final
%type <body> body
%type <type> type

%{
//-- The rules below will be included in yyparse, the main parsing function.
%}
%%

file         : globals              { compiler->ast(new at::file_node(LINE, $1)); }
             ;

globals      : global               { $$ = new cdk::sequence_node(LINE, $1); }
             | globals global       { $$ = new cdk::sequence_node(LINE, $2, $1); }
             ;

/*-------------------------------- declarations --------------------------------*/

global       : tPUBLIC type tIDENTIFIER opt_init ';'        { $$ = new at::variable_declaration_node(LINE, tPUBLIC, false, $2, *$3, $4); delete $3; }
             | tCONST tPUBLIC type tIDENTIFIER opt_init ';' { $$ = new at::variable_declaration_node(LINE, tPUBLIC, true, $3, *$4, $5); delete $4; }
             | tUSE type tIDENTIFIER ';'                    { $$ = new at::variable_declaration_node(LINE, tUSE, false, $2, *$3, nullptr); delete $3; }
             | tCONST tUSE type tIDENTIFIER ';'             { $$ = new at::variable_declaration_node(LINE, tUSE, true, $3, *$4, nullptr); delete $4; }
             | var ';'                                      { $$ = $1; }
             | tPUBLIC type tIDENTIFIER '(' arguments ')' opt_literal       { $$ = new at::function_declaration_node(LINE, tPUBLIC, $2, *$3, $5, $7); delete $3; }
             | tPUBLIC type tIDENTIFIER '(' arguments ')' opt_literal body  { $$ = new at::function_definition_node(LINE, tPUBLIC, $2, *$3, $5, $7, $8); delete $3; }
             | tUSE type tIDENTIFIER '(' arguments ')' opt_literal          { $$ = new at::function_declaration_node(LINE, tUSE, $2, *$3, $5, $7); delete $3; }
             | tUSE type tIDENTIFIER '(' arguments ')' opt_literal body     { $$ = new at::function_definition_node(LINE, tUSE, $2, *$3, $5, $7, $8); delete $3; }
             | function                                     { $$ = $1; }
             ;

function     : type tIDENTIFIER '(' arguments ')' opt_literal        { $$ = new at::function_declaration_node(LINE, 0, $1, *$2, $4, $6); delete $2; }
             | type tIDENTIFIER '(' arguments ')' opt_literal body   { $$ = new at::function_definition_node(LINE, 0, $1, *$2, $4, $6, $7); delete $2; }
             ;

var          : type tIDENTIFIER opt_init          { $$ = new at::variable_declaration_node(LINE, 0, false, $1, *$2, $3); delete $2; }
             | tCONST type tIDENTIFIER opt_init   { $$ = new at::variable_declaration_node(LINE, 0, true, $2, *$3, $4); delete $3; }
             ;

opt_init     : /* empty */          { $$ = nullptr; }
             | '=' expr             { $$ = $2; }
             ;

arguments    : /* empty */             { $$ = EMPTY; }
             | argument                { $$ = new cdk::sequence_node(LINE, $1); }
             | arguments ',' argument  { $$ = new cdk::sequence_node(LINE, $3, $1); }
             ;

argument     : type tIDENTIFIER                { $$ = new at::variable_declaration_node(LINE, 0, false, $1, *$2, nullptr); delete $2; }
             | type tIDENTIFIER '=' literal    { $$ = new at::variable_declaration_node(LINE, 0, false, $1, *$2, $4); delete $2; }
             ;

opt_literal  : /* empty */          { $$ = nullptr; }
             | tFUNCTION literal    { $$ = $2; }
             ;

literal      : tINTEGER             { $$ = new cdk::integer_node(LINE, $1); }
             | tREAL                { $$ = new cdk::double_node(LINE, $1); }
             | tTEXT                { $$ = new cdk::string_node(LINE, $1); delete $1; }
             ;

type         : tTYPE_INT            { $$ = cdk::primitive_type::create(4, cdk::TYPE_INT); }
             | tTYPE_REAL           { $$ = cdk::primitive_type::create(8, cdk::TYPE_DOUBLE); }
             | tTYPE_STRING         { $$ = cdk::primitive_type::create(4, cdk::TYPE_STRING); }
             | tTYPE_VOID           { $$ = cdk::primitive_type::create(0, cdk::TYPE_VOID); }
             | '<' type '>'         { $$ = cdk::reference_type::create(4, $2); }
             ;

/*------------------------------------ bodies ------------------------------------*/

body         : initial                  { $$ = new at::body_node(LINE, $1, EMPTY, nullptr); }
             | sections                 { $$ = new at::body_node(LINE, nullptr, $1, nullptr); }
             | final                    { $$ = new at::body_node(LINE, nullptr, EMPTY, $1); }
             | initial sections         { $$ = new at::body_node(LINE, $1, $2, nullptr); }
             | initial final            { $$ = new at::body_node(LINE, $1, EMPTY, $2); }
             | sections final           { $$ = new at::body_node(LINE, nullptr, $1, $2); }
             | initial sections final   { $$ = new at::body_node(LINE, $1, $2, $3); }
             ;

initial      : tOPENBODY block          { $$ = $2; }
             ;

final        : tCLOSEBODY block         { $$ = $2; }
             ;

sections     : section                  { $$ = new cdk::sequence_node(LINE, $1); }
             | sections section         { $$ = new cdk::sequence_node(LINE, $2, $1); }
             ;

section      : block                    { $$ = new at::section_node(LINE, false, nullptr, $1); }
             | '(' ')' block            { $$ = new at::section_node(LINE, false, nullptr, $3); }
             | '(' expr ')' block       { $$ = new at::section_node(LINE, false, $2, $4); }
             | '[' ']' block            { $$ = new at::section_node(LINE, true, nullptr, $3); }
             | '[' expr ']' block       { $$ = new at::section_node(LINE, true, $2, $4); }
             ;

block        : '{' '}'                           { $$ = new at::block_node(LINE, EMPTY, EMPTY); }
             | '{' declarations '}'              { $$ = new at::block_node(LINE, $2, EMPTY); }
             | '{' instructions '}'              { $$ = new at::block_node(LINE, EMPTY, $2); }
             | '{' declarations instructions '}' { $$ = new at::block_node(LINE, $2, $3); }
             ;

declarations : declaration                { $$ = new cdk::sequence_node(LINE, $1); }
             | declarations declaration   { $$ = new cdk::sequence_node(LINE, $2, $1); }
             ;

declaration  : var ';'                    { $$ = $1; }
             | function ';'               { $$ = $1; }
             ;

/*---------------------------------- instructions ----------------------------------*/

instructions : instruction                { $$ = new cdk::sequence_node(LINE, $1); }
             | instructions instruction   { $$ = chain_conditional($1->nodes().back(), $2) ? $1 : new cdk::sequence_node(LINE, $2, $1); }
             ;

instruction  : ';'                        { $$ = new cdk::nil_node(LINE); }
             | expr ';'                   { $$ = new at::evaluation_node(LINE, $1); }
             | expr '!'                   { $$ = new at::print_node(LINE, $1, false); }
             | expr tPRINTLN              { $$ = new at::print_node(LINE, $1, true); }
             | tNEXT                      { $$ = new at::next_node(LINE); }
             | tSTOP                      { $$ = new at::stop_node(LINE); }
             | tRETURN                    { $$ = new at::return_node(LINE); }
             | conditional                { $$ = $1; }
             | iteration                  { $$ = $1; }
             | block                      { $$ = $1; }
             ;

/* Consecutive "[c] ? i" instructions chain into an if / else-if ladder,
 * which a final ": i" closes (see chain_conditional above). */
conditional  : '[' expr ']' '#' instruction                 { $$ = new at::if_node(LINE, $2, $5); }
             | '[' expr ']' '?' instruction %prec tIFX      { $$ = new at::if_node(LINE, $2, $5, true); }
             | '[' expr ']' '?' instruction ':' instruction { $$ = new at::if_else_node(LINE, $2, $5, $7); }
             ;

iteration    : '[' opt_expr ';' opt_expr ';' opt_expr ']' instruction { $$ = new at::for_node(LINE, nullptr, $2, $4, $6, $8); }
             | '[' vars ';' opt_expr ';' opt_expr ']' instruction     { $$ = new at::for_node(LINE, $2, nullptr, $4, $6, $8); }
             ;

vars         : var                      { $$ = new cdk::sequence_node(LINE, $1); }
             | vars ',' var             { $$ = new cdk::sequence_node(LINE, $3, $1); }
             ;

/*---------------------------------- expressions ----------------------------------*/

opt_expr     : /* empty */              { $$ = nullptr; }
             | expr                     { $$ = $1; }
             ;

exprs        : expr                     { $$ = new cdk::sequence_node(LINE, $1); }
             | exprs ',' expr           { $$ = new cdk::sequence_node(LINE, $3, $1); }
             ;

opt_exprs    : /* empty */              { $$ = EMPTY; }
             | exprs                    { $$ = $1; }
             ;

invocation   : tIDENTIFIER '(' opt_exprs ')'  { $$ = new at::function_call_node(LINE, *$1, $3); delete $1; }
             | '@' '(' opt_exprs ')'          { $$ = new at::function_call_node(LINE, "@", $3); }
             ;

nested       : '@' '.' tIDENTIFIER      { $$ = new at::nested_variable_node(LINE, *$3, 1); delete $3; }
             | '@' '.' '@'              { $$ = new at::nested_variable_node(LINE, "@", 1); }
             | '@' '.' nested           { $$ = new at::nested_variable_node(LINE, $3->name(), $3->depth() + 1); delete $3; }
             ;

index        : tIDENTIFIER '[' expr ']'   { $$ = new at::index_node(LINE, new cdk::rvalue_node(LINE, new cdk::variable_node(LINE, $1)), $3); delete $1; }
             | '(' expr ')' '[' expr ']'  { $$ = new at::index_node(LINE, $2, $5); }
             | invocation '[' expr ']'    { $$ = new at::index_node(LINE, $1, $3); }
             | index '[' expr ']'         { $$ = new at::index_node(LINE, new cdk::rvalue_node(LINE, $1), $3); }
             ;

lvalue       : tIDENTIFIER              { $$ = new cdk::variable_node(LINE, $1); delete $1; }
             | nested                   { $$ = $1; }
             | index                    { $$ = $1; }
             | '@'                      { $$ = new cdk::variable_node(LINE, "@"); }
             ;

expr         : string                   { $$ = $1; }
             | tREAL                    { $$ = new cdk::double_node(LINE, $1); }
             | tINTEGER                 { $$ = new cdk::integer_node(LINE, $1); }
             | tIDENTIFIER              { $$ = new cdk::rvalue_node(LINE, new cdk::variable_node(LINE, $1)); delete $1; }
             | nested                   { $$ = new cdk::rvalue_node(LINE, $1); }
             | index                    { $$ = new cdk::rvalue_node(LINE, $1); }
             | lvalue '?'               { $$ = new at::address_of_node(LINE, $1); }
             | lvalue '=' expr          { $$ = new cdk::assignment_node(LINE, $1, $3); }
             | '@'                      { $$ = new at::read_node(LINE); }
             | invocation               { $$ = $1; }
             | '(' expr ')'             { $$ = $2; }
             | '#' expr                 { $$ = new at::alloc_node(LINE, $2); }
             | '+' expr %prec tUMINUS   { $$ = new cdk::unary_plus_node(LINE, $2); }
             | '-' expr %prec tUMINUS   { $$ = new cdk::unary_minus_node(LINE, $2); }
             | '~' expr                 { $$ = new cdk::not_node(LINE, $2); }
             | expr '+' expr            { $$ = new cdk::add_node(LINE, $1, $3); }
             | expr '-' expr            { $$ = new cdk::sub_node(LINE, $1, $3); }
             | expr '*' expr            { $$ = new cdk::mul_node(LINE, $1, $3); }
             | expr '/' expr            { $$ = new cdk::div_node(LINE, $1, $3); }
             | expr '%' expr            { $$ = new cdk::mod_node(LINE, $1, $3); }
             | expr '^' expr            { $$ = new at::power_node(LINE, $1, $3); }
             | expr '<' expr            { $$ = new cdk::lt_node(LINE, $1, $3); }
             | expr '>' expr            { $$ = new cdk::gt_node(LINE, $1, $3); }
             | expr tGE expr            { $$ = new cdk::ge_node(LINE, $1, $3); }
             | expr tLE expr            { $$ = new cdk::le_node(LINE, $1, $3); }
             | expr tNE expr            { $$ = new cdk::ne_node(LINE, $1, $3); }
             | expr tEQ expr            { $$ = new cdk::eq_node(LINE, $1, $3); }
             | expr tAND expr           { $$ = new cdk::and_node(LINE, $1, $3); }
             | expr tOR expr            { $$ = new cdk::or_node(LINE, $1, $3); }
             ;

/* Adjacent strings, characters and integers (taken as character codes)
 * make one string: "a" 'b' 10 is "ab\n". */
string       : tTEXT                    { $$ = new cdk::string_node(LINE, $1); delete $1; }
             | tTEXT strbody            { $$ = new cdk::string_node(LINE, *$1 + *$2); delete $1; delete $2; }
             | tCHAR strbody            { $$ = new cdk::string_node(LINE, *$1 + *$2); delete $1; delete $2; }
             | tINTEGER strbody         { $$ = new cdk::string_node(LINE, std::string(1, (char)$1) + *$2); delete $2; }
             ;

strbody      : tTEXT                    { $$ = $1; }
             | tCHAR                    { $$ = $1; }
             | tINTEGER                 { $$ = new std::string(1, (char)$1); }
             | strbody tTEXT            { $$ = $1; *$$ += *$2; delete $2; }
             | strbody tCHAR            { $$ = $1; *$$ += *$2; delete $2; }
             | strbody tINTEGER         { $$ = $1; *$$ += (char)$2; }
             ;

%%
