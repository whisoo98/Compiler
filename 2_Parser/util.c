/****************************************************/
/* File: util.c                                     */
/* Utility function implementation                  */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"
#include <string.h>
/* Procedure printToken prints a token
 * and its lexeme to the listing file
 */
void printToken(TokenType token, const char *tokenString)
{
  switch (token)
  {
  case IF:
    fprintf(listing, "reserved word: if\n");
    break;
  // case THEN:
  case ELSE:
    fprintf(listing, "reserved word: else\n");
    break;
  case WHILE:
    fprintf(listing, "reserved word: while\n");
    break;
  case RETURN:
    fprintf(listing, "reserved word: return\n");
    break;
  case INT:
    fprintf(listing, "reserved word: int\n");
    break;
  case VOID:
    fprintf(listing, "reserved word: void\n");
    break;
    // case END:
    // case REPEAT:
    // case UNTIL:
    // case READ:
    // case WRITE:
    /*fprintf(listing,
       "reserved word: %s\n",tokenString);
    break;*/
    //,NE,,LE,GT,GE,,,,,,,LBRACE,RBRACE,LCURLY,RCULRY,,COMMA
  case ASSIGN:
    fprintf(listing, "=\n");
    break;
  case EQ:
    fprintf(listing, "==\n");
    break;
  case NE:
    fprintf(listing, "!=\n");
    break;
  case LT:
    fprintf(listing, "<\n");
    break;
  case LE:
    fprintf(listing, "<=\n");
    break;
  case GT:
    fprintf(listing, ">\n");
    break;
  case GE:
    fprintf(listing, ">=\n");
    break;
  case PLUS:
    fprintf(listing, "+\n");
    break;
  case MINUS:
    fprintf(listing, "-\n");
    break;
  case TIMES:
    fprintf(listing, "*\n");
    break;
  case OVER:
    fprintf(listing, "/\n");
    break;
  case LPAREN:
    fprintf(listing, "(\n");
    break;
  case RPAREN:
    fprintf(listing, ")\n");
    break;
  case LBRACE:
    fprintf(listing, "[\n");
    break;
  case RBRACE:
    fprintf(listing, "]\n");
    break;
  case LCURLY:
    fprintf(listing, "{\n");
    break;
  case RCURLY:
    fprintf(listing, "}\n");
    break;
  case SEMI:
    fprintf(listing, ";\n");
    break;
  case COMMA:
    fprintf(listing, ",\n");
    break;

  case ENDFILE:
    fprintf(listing, "EOF\n");
    break;
  case NUM:
    fprintf(listing,
            "NUM, val= %s\n", tokenString);
    break;
  case ID:
    fprintf(listing,
            "ID, name= %s\n", tokenString);
    break;
  case ERROR:
    fprintf(listing,
            "ERROR: %s\n", tokenString);
    break;
  default: /* should never happen */
    fprintf(listing, "Unknown token: %d\n", token);
  }
}

/* Function newDclrKind creates a new declaration
 * node for syntax tree construction
 */
TreeNode *newDclrNode(DclrKind kind)
{
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else
  {
    for (i = 0; i < MAXCHILDREN; i++)
      t->child[i] = NULL;
    t->sibling = NULL;
    t->lineno = lineno;
    t->nodekind = DclrK;
    t->kind.dclr = kind;
  }
  return t;
}

/* Function newParamNode creates a new parameter
 * node for syntax tree construction
 */
TreeNode *newParamNode(ParamKind kind)
{
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else
  {
    for (i = 0; i < MAXCHILDREN; i++)
      t->child[i] = NULL;
    t->sibling = NULL;
    t->lineno = lineno;
    t->nodekind = ParamK;
    t->kind.prm = kind;
  }
  return t;
}

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
TreeNode *newStmtNode(StmtKind kind)
{
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else
  {
    for (i = 0; i < MAXCHILDREN; i++)
      t->child[i] = NULL;
    t->sibling = NULL;
    t->lineno = lineno;
    t->nodekind = StmtK;
    t->kind.stmt = kind;
  }
  return t;
}

/* Function newExpNode creates a new expression
 * node for syntax tree construction
 */
TreeNode *newExpNode(ExpKind kind)
{
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else
  {
    for (i = 0; i < MAXCHILDREN; i++)
      t->child[i] = NULL;
    t->sibling = NULL;
    t->lineno = lineno;
    t->nodekind = ExpK;
    t->kind.exp = kind;
  }
  return t;
}

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char *copyString(char *s)
{
  int n;
  char *t;
  if (s == NULL)
    return NULL;
  n = strlen(s) + 1;
  t = malloc(n);
  if (t == NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else
    strcpy(t, s);
  return t;
}

/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno += 2
#define UNINDENT indentno -= 2

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{
  int i;
  for (i = 0; i < indentno; i++)
    fprintf(listing, " ");
}

/* procedure printTree prints a syntax tree to the
 * listing file using indentation to indicate subtrees
 */
void printTree(TreeNode *tree)
{
  int i;
  INDENT;
  while (tree != NULL)
  {
    printSpaces();
    if (tree->nodekind == StmtK)
    {
      switch (tree->kind.stmt)
      {
      case IfK:
        fprintf(listing, "If Statement:\n");
        break;
      case ElseK:
        fprintf(listing, "If-Else Statement:\n");
        break;
      case WhileK:
        fprintf(listing, "While Statement:\n");
        break;
      case NonReturnK:
        fprintf(listing, "Non-value Return Statement\n");
        break;
      case ReturnK:
        fprintf(listing, "Return Statement:\n");
        break;
      case CompK:
        fprintf(listing, "Compound Statement:\n");
        break;
      case AssignK:
        fprintf(listing, "Assign:\n");
        break;
      default:
        fprintf(listing, "Unknown ExpNode kind\n");
        break;
      }
    }
    else if (tree->nodekind == ExpK)
    {
      switch (tree->kind.exp)
      {
      case OpK:
        fprintf(listing, "Op: ");
        printToken(tree->attr.op, "\0");
        break;
      case ConstK:
        fprintf(listing, "Const: %d\n", tree->attr.val);
        break;
      case IdK:
        fprintf(listing, "Variable: name = %s\n", tree->attr.name);
        break;
      case ArrEK:
        fprintf(listing, "Variable: name = %s\n", tree->attr.name);
        break;
      case CallK:
        fprintf(listing, "Call: function name = %s\n", tree->attr.name);
        break;
      default:
        fprintf(listing, "Unknown ExpNode kind\n");
        break;
      }
    }
    else if (tree->nodekind == DclrK)
    {
      char *type;
      if (tree->type == Void)
        type = "void";
      else if (tree->type == Integer)
        type = "int";
      else if (tree->type == Null)
        type = "null";
      else
        fprintf(listing, "Unknown ExpType\n");
      switch (tree->kind.dclr)
      {
      case VarK:
        fprintf(listing, "Variable Declaration: name = %s, type = %s\n", tree->attr.name, type);
        break;
      case ArrPK:
        fprintf(listing, "Variable Declaration: name = %s, type = %s[]\n", tree->attr.name, type);
        break;
      case FuncK:
        fprintf(listing, "Function Declaration: name = %s, return type = %s\n", tree->attr.name, type);
        break;
      case TypeK:
        fprintf(listing, "Type Declaration: Don't Print\n");
        break;
      default:
        fprintf(listing, "Unknown DclrNode kind\n");
        break;
      }
    }
    else if (tree->nodekind == ParamK)
    {
      char *type;
      if (tree->type == Void)
        type = "void";
      else if (tree->type == Integer)
        type = "int";
      else if (tree->type == Null)
        type = "null";
      else
        fprintf(listing, "Unknown ExpType\n");
      switch (tree->kind.prm)
      {
      case NArrK:
        fprintf(listing, "Parameter: name = %s, type = %s\n", tree->attr.name, type);
        break;
      case ArrPK:
        fprintf(listing, "Parameter: name = %s, type = %s[]\n", tree->attr.name, type);
        break;
      case NullK:
        fprintf(listing, "Void Parameter\n");
        break;
      default:
        fprintf(listing, "Unknown ParamNode kind\n");
        break;
      }
    }
    else
      fprintf(listing, "Unknown node kind\n");
    for (i = 0; i < MAXCHILDREN; i++)
      printTree(tree->child[i]);
    tree = tree->sibling;
  }
  UNINDENT;
}
