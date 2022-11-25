/****************************************************/
/* File: tiny.y                                     */
/* The TINY Yacc/Bison specification file           */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/
%{
#define YYPARSER /* distinguishes Yacc output from other code files */

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

#define YYSTYPE TreeNode *
static char * savedName; /* for use in assignments */
static int savedLineNo;  /* ditto */
static TreeNode * savedTree; /* stores syntax tree for later return */
static int yylex(void); // added 11/2/11 to ensure no conflict with lex

%} 
/* Define Section - Cleared */
/* Priority  Top < Bottom */
/* Associativity  %left, %right, %nonassoc instead of %token */



%nonassoc  SEMI   /* What Precedence of SEMI ????????????????????????? */
%left COMMA
%right ASSIGN
%left EQ NE
%left LT LE GT GE
%left PLUS MINUS
%left TIMES OVER
/*%left LPAREN  LCURLY  LBRACE 
%right RPAREN RCURLY RBRACE*/
%left LPAREN LCURLY LBRACE  RCURLY RBRACE
%nonassoc ID NUM /* identifier & number */
%left INT IF  VOID WHILE RETURN /* Reserved Keywords */
%nonassoc RPAREN
%nonassoc ELSE

%nonassoc ERROR /* ERROR  */

%% /* Grammar for TINY -> modifing to Grammar for C-Minus */
program     : declaration_list // Done 1
                 { savedTree = $1;} 
            ;
declaration_list    : declaration_list declaration // Done 2
                            { 
                              YYSTYPE t = $1;
                              if (t != NULL){
                                while (t->sibling != NULL) t = t->sibling;
                                t -> sibling = $2;
                                $$ = $1;
                              }
                              else {$$ = $2;}
                            }
                            | declaration { $$ = $1; }
                            ;
declaration        : var_declaration
                            { // Done 3
                              $$ = $1;
                            }
                            | func_declaration
                            {
                              $$ = $1;
                            }
                            ;
var_declaration  : type_specifier id SEMI // where is the lineno
                            { // Done 4
                              $$ = newDclrNode(VarK);
                              $$->lineno = $1->lineno; 
                              $$->attr.name=$2->attr.name;
                              $$->type = $1->type;
                              /* Do I have to save the ID? */
                            }
                            | type_specifier id LBRACE num RBRACE SEMI
                            {
                              $$ = newDclrNode(ArrK);
                              $$->lineno = $1->lineno; 
                              $$->type = $1->type;
                              $$->attr.name=$2->attr.name;
                              $$->child [0]=$4;
                            }
                            ;
type_specifier    : INT  // Done 5
                            {
                              $$ = newDclrNode(TypeK);
                              $$->lineno = lineno;
                              $$->type = Integer;
                            }
                            | VOID
                            {
                              $$ = newDclrNode(TypeK);
                              $$->lineno = lineno;
                              $$->type = Void;
                            }
                            ;
func_declaration : type_specifier id LPAREN params RPAREN compound_stmt  // Done 6
                            {
                              $$ = newDclrNode(FuncK);
                              $$->child[0] = $4;
                              $$->child[1] = $6;
                              $$->lineno = $1->lineno; 
                              $$->attr.name = $2->attr.name;
                              $$->type = $1->type;
                            }
                            ;
params                : param_list // Done 7
                            {
                              $$ = $1;
                            }
                            | VOID // Is it Right?
                            {
                              $$ = newParamNode(NullK);
                              $$->lineno = lineno; 
                              $$->type = Null;
                            }
                            ;
param_list            : param_list COMMA param // Done 8
                            { 
                              YYSTYPE t = $1;
                              if (t != NULL){
                                while (t->sibling != NULL) t = t->sibling;
                                t -> sibling = $3;  
                                $$ = $1;
                              }
                              else {$$ = $3;}
                            }
                            | param
                            {
                              $$ = $1;
                            }
                            ;
param                : type_specifier id // Done 9
                            {
                              $$ = newParamNode(NArrK);
                              $$->lineno = $1->lineno; 
                              $$->attr.name = $2->attr.name;
                              $$->type = $1 ->type;
                            }
                            | type_specifier id LBRACE RBRACE
                            {
                              $$ = newParamNode(ArrPK);
                              $$->lineno = $1->lineno; 
                              $$->attr.name = $2->attr.name;
                              $$->type = $1 ->type;
                            }
                            ;
compound_stmt : lcurly local_declarations statement_list RCURLY // Done 10
                            {
                              $$ = newStmtNode(CompK);
                              $$ -> child[0] = $2;
                              $$ -> child[1] = $3;
                              $$->lineno = $1-> lineno;
                            }
                            ;
local_declarations : local_declarations var_declaration // Done 11
                            { 
                              YYSTYPE t = $1;
                              if (t != NULL){
                                while (t->sibling != NULL) t = t->sibling;
                                t -> sibling = $2;
                                $$ = $1;
                              }
                              else {$$ = $2;}
                            }
                            | 
                            {
                              $$ = NULL;
                            }
                            ;
statement_list    : statement_list statement // Done 12
                            { 
                              YYSTYPE t = $1;
                              if (t != NULL){
                                while (t->sibling != NULL) t = t->sibling;
                                t -> sibling = $2;
                                $$ = $1;
                              }
                              else {$$ = $2;}
                            }
                            |
                            {
                              $$ = NULL;
                            }
                            ;
statement          : expression_stmt // Done 13
                            {
                              $$ = $1;
                            }
                            |
                            compound_stmt
                            {
                              $$ = $1;
                            }
                            |
                            selection_stmt
                            {
                              $$ = $1;
                            }
                            |
                            iteration_stmt
                            {
                              $$ = $1;
                            }
                            |
                            return_stmt
                            {
                              $$ = $1;
                            }
                            ;
expression_stmt : expression SEMI // Done 14
                             {
                              $$ = $1;
                             }
                             | SEMI 
                             {
                              $$ = NULL;
                             }
                             ;
selection_stmt   : If LPAREN expression RPAREN statement // Done 15
                            {
                              $$ = newStmtNode(IfK);
                              $$ -> child[0] = $3;
                              $$ -> child[1] = $5;
                              $$ -> lineno = $1->lineno;
                            }
                            |
                            If LPAREN expression RPAREN statement ELSE statement
                            {
                              $$ = newStmtNode(ElseK);
                              $$ -> child[0] = $3;
                              $$ -> child[1] = $5;
                              $$ -> child[2] = $7;
                              $$ -> lineno = $1->lineno;
                            }
                            ;
iteration_stmt    : While LPAREN expression RPAREN statement // Done 16
                            {
                              $$ = newStmtNode(WhileK);
                              $$->child[0] = $3;
                              $$->child[1] = $5;
                              $$ -> lineno = $1->lineno;
                            }
                            ;
return_stmt        : Return SEMI // Done 17
                            {
                              $$ = newStmtNode(NonReturnK);
                              $$ -> lineno = $1->lineno;
                            }
                            |
                            Return expression SEMI
                            {
                              $$ = newStmtNode(ReturnK);
                              $$ -> child[0] = $2;
                              $$ -> lineno = $1->lineno;
                            }
                            ;
expression         : var ASSIGN expression // Done 18
                            {
                              $$ = newStmtNode(AssignK);
                              $$-> child[0] = $1;
                              $$-> child[1] = $3;
                              $$ -> lineno = $1->lineno;
                            }
                            |
                            simple_expression
                            {
                              $$=$1;
                            }
                            ;
var                      : id // Done 19
                            {
                              $$ = newExpNode(IdK);
                              $$ -> lineno = $1->lineno;
                              $$-> attr.name = $1->attr.name;
                            }
                            | id LBRACE expression RBRACE
                            {
                              $$ = newExpNode(ArrEK);
                              $$->child[0] = $3;
                              $$ -> lineno = $1->lineno;
                              $$-> attr.name = $1->attr.name;
                            }
                            ;
simple_expression : additive_expression relop additive_expression //  Done 20
                            {
                              $$ = newExpNode(OpK);
                              $$-> child[0] = $1;
                              $$-> child[1] = $3;
                              $$ -> lineno = $1->lineno;
                              $$->attr.op = $2->attr.op;
                            }
                            |
                            additive_expression
                            {
                              $$ = $1;
                            }
                            ;
relop                  : LE // Done 21
                            {
                              $$ = newExpNode(OpK);
                              $$->lineno = lineno;
                              $$->attr.op = LE;
                            }
                            | 
                            LT
                            {
                              $$ = newExpNode(OpK);
                              $$->lineno = lineno;
                              $$->attr.op = LT;
                            }
                            | 
                            GT
                            {
                              $$ = newExpNode(OpK);
                              $$->lineno = lineno;
                              $$->attr.op = GT;
                            }
                            | 
                            GE
                            {
                              $$ = newExpNode(OpK);
                              $$->lineno = lineno;
                              $$->attr.op = GE;
                            }
                            | 
                            EQ
                            {
                              $$ = newExpNode(OpK);
                              $$->lineno = lineno;
                              $$->attr.op = EQ;
                            }
                            | 
                            NE
                            {
                              $$ = newExpNode(OpK);
                              $$->lineno = lineno;
                              $$->attr.op = NE;
                            }
                            ;
additive_expression : additive_expression addop term //  Done 22
                            {
                              $$ = newExpNode(OpK);
                              $$-> child[0] = $1;
                              $$-> child[1] = $3;
                              $$->lineno = $1->lineno;
                              $$->attr.op = $2->attr.op;
                            }
                            |
                            term
                            {
                              $$ = $1;
                            }
                            ;
addop                  : PLUS // Done 23
                            {
                              $$ = newExpNode(OpK);
                              $$->lineno = lineno;
                              $$->attr.op = PLUS;
                            }
                            | 
                            MINUS
                            {
                              $$ = newExpNode(OpK);
                              $$->lineno = lineno;
                              $$->attr.op = MINUS;
                            }
                            ;
term                     : term mulop factor //  Done 24
                            {
                              $$ = newExpNode(OpK);
                              $$-> child[0] = $1;
                              $$-> child[1] = $3;
                              $$->lineno = $1->lineno;
                              $$->attr.op = $2->attr.op;
                            }
                            |
                            factor
                            {
                              $$ = $1;
                            }
                            ;
mulop                  : TIMES // Done 25
                            {
                              $$ = newExpNode(OpK);
                              $$->lineno = lineno;
                              $$->attr.op = TIMES;
                            }
                            | 
                            OVER
                            {
                              $$ = newExpNode(OpK);
                              $$->lineno = lineno;
                              $$->attr.op = OVER;
                            }
                            ;
factor                   : LPAREN expression RPAREN//  Done 26
                            {
                              $$ = $2;
                            }
                            |
                            var
                            {
                              $$ = $1;
                            }
                            |
                            call
                            {
                              $$ = $1;
                            }
                            |
                            num
                            {
                              $$ = newExpNode(ConstK);
                              $$->lineno = $1->lineno;
                              $$->attr.val = $1->attr.val;
                            }
                            ;
call                     : id LPAREN args RPAREN // Done 27
                            {
                              $$=newExpNode(CallK);
                              $$->child[0] = $3;
                              $$->lineno = $1->lineno;
                              $$->attr.name = $1->attr.name;
                            }
                            ;
args                    : arg_list // Done 28
                             {
                              $$=$1;
                             }
                             |
                             {
                              $$= NULL;
                             }
                             ;
arg_list               : arg_list COMMA expression // Done 29
                            { YYSTYPE t = $1;
                              if (t != NULL) 
                              { 
                                while (t->sibling != NULL)
                                    t = t->sibling;
                                t->sibling = $3;
                                $$ = $1; 
                              }
                              else {$$ = $3;} 
                            }
                            |
                            expression
                            {
                              $$ = $1;
                            }
                            ;
id                      : ID
                          {
                            $$ = newExpNode(IdK);
                            $$->lineno = lineno;
                            $$->attr.name = copyString(tokenString);
                          }
                          ;
num                : NUM
                          {
                            $$ = newExpNode(ConstK);
                            $$->lineno = lineno;
                            $$->attr.val = atoi(tokenString);
                          } 
                          ;
lcurly            : LCURLY
                          {
                            $$ = newExpNode(OpK);
                            $$->lineno = lineno;
                            $$->attr.op = LCURLY;
                          } 
                          ;
If                     : IF
                          {
                            $$ = newStmtNode(IfK);
                            $$->lineno = lineno;
                            $$->attr.op = IF;
                          } 
                          ;
While             : WHILE
                          {
                            $$ = newStmtNode(WhileK);
                            $$->lineno = lineno;
                            $$->attr.op = WHILE;
                          } 
                          ;
Return           : RETURN
                          {
                            $$ = newStmtNode(ReturnK);
                            $$->lineno = lineno;
                            $$->attr.op = RETURN;
                          } 
                          ;
%%

int yyerror(char * message)
{ fprintf(listing,"Syntax error at line %d: %s\n",lineno,message);
  fprintf(listing,"Current token: ");
  printToken(yychar,tokenString);
  Error = TRUE;
  savedTree=NULL;
  return 0;
}

/* yylex calls getToken to make Yacc/Bison output
 * compatible with ealier versions of the TINY scanner
 */
static int yylex(void)
{ return getToken(); }

TreeNode * parse(void)
{ 
  yyparse();
  return savedTree;
}

