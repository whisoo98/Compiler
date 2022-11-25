/****************************************************/
/* File: analyze.c                                  */
/* Semantic analyzer implementation                 */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "symtab.h"
#include "analyze.h"
#include <stdio.h>
/* counter for variable memory locations */
static int location = 0;

typedef struct Stack
{
  char *scope;
  struct Stack *parent;
} *StackPtr;
StackPtr top = NULL;

void make_header()
{
  if (top == NULL)
  {
    StackPtr tmp = (StackPtr)malloc(sizeof(struct Stack));
    tmp->scope = "global";
    tmp->parent = NULL;
    top = tmp;
    make_table(NULL, top->scope);
    st_insert("global", "input", Integer, 0, 0);
    st_insert("global", "output", Void, 0, 0);
  }
}

static void cycle()
{
  StackPtr tmp = top;
  while (tmp != NULL)
  {
    tmp = tmp->parent;
  }
}
static void push(char *scope, int lineno)
{
  char *newscope = (char *)malloc(sizeof(char) * 1000);
  sprintf(newscope, "%s%d", scope, lineno);

  make_table(top->scope, newscope);

  StackPtr tmp = (StackPtr)malloc(sizeof(struct Stack));
  tmp->scope = newscope;
  tmp->parent = top;
  top = tmp;

  cycle();
};

static void pop(char *scope)
{
  cycle();
  cycle();
  StackPtr tmp = top;
  top = top->parent;
  delete_table(top->scope, scope);

  if (top == NULL)
  {
    top = tmp;
    return;
  }
  free(tmp->scope);
  free(tmp);
}
/* Procedure traverse is a generic recursive
 * syntax tree traversal routine:
 * it applies preProc in preorder and postProc
 * in postorder to tree pointed to by t
 */
static void traverse(TreeNode *t,
                     void (*preProc)(TreeNode *),
                     void (*postProc)(TreeNode *))
{
  if (t != NULL)
  {
    preProc(t);
    {
      int i;
      for (i = 0; i < MAXCHILDREN; i++)
      {
        // printf("i : %d\n",i);
        traverse(t->child[i], preProc, postProc);
      }
    }
    // printf("bot\n");
    postProc(t);
    traverse(t->sibling, preProc, postProc);
    if ((t->nodekind == StmtK && t->kind.stmt == CompK) || (t->nodekind == DclrK && t->kind.dclr == FuncK))
    {
      pop(top->scope);
    }
  }
}

/* nullProc is a do-nothing procedure to
 * generate preorder-only or postorder-only
 * traversals from traverse
 */
static void nullProc(TreeNode *t)
{
  if (t == NULL)
    return;
  else
    return;
}

/* Procedure insertNode inserts
 * identifiers stored in t into
 * the symbol table
 */

// need to MODIFY
static void insertNode(TreeNode *t)
{
  if (t == NULL)
  {
    return;
  }

  switch (t->nodekind)
  {

  case StmtK:
    switch (t->kind.stmt)
    {

    case IfK:

      break;
    case ElseK:
      break;
    case WhileK:
      break;
    case ReturnK:
      break;
    case NonReturnK:
      break;
    case CompK:
      push(top->scope, t->lineno);
      break;
    case AssignK:
      break;
    default:
      break;
    }
    break;
  case ExpK:
    switch (t->kind.exp)
    {

    case OpK:
      break;
    case ConstK:
      break;
    case IdK:
      break;
    case ArrEK:
      break;
    case CallK:
      break;
    default:
      break;
    }
    break;
  case DclrK: // cleared
    switch (t->kind.dclr)
    {

    case VarK: // cleared
      // int Type = st_lookup_excluding_parent(top->scope, t->attr.name);
      if (st_lookup_excluding_parent(top->scope, t->attr.name) != -1)
        /* not yet in table, so treat as new definition */
        fprintf(listing, "Error: Symbol \"%s\" is redefined at line %d\n", t->attr.name, t->lineno);
      else if (t->type == Void)
      {

        fprintf(listing, "Error: The void-type variable is declared at line %d (name : \"%s\")\n", t->lineno, t->attr.name);
        st_insert(top->scope, t->attr.name, t->type, t->lineno, location++);
      }
      else

        /* already in table, so ignore location,
           add line number of use only */
        st_insert(top->scope, t->attr.name, t->type, t->lineno, location++);
      break;
    case ArrK: // cleared
      if (st_lookup_excluding_parent(top->scope, t->attr.name) != -1)
        /* not yet in table, so treat as new definition */
        fprintf(listing, "Error: Symbol \"%s\" is redefined at line %d\n", t->attr.name, t->lineno);
      else if (t->type != IntArr)
      {
        fprintf(listing, "Error: The void-type variable is declared at line %d (name : \"%s\")\n", t->lineno, t->attr.name);
        st_insert(top->scope, t->attr.name, t->type, t->lineno, location++);
      }
      else
        /* already in table, so ignore location,
           add line number of use only */
        st_insert(top->scope, t->attr.name, t->type, t->lineno, location++);
      break;
    case FuncK: // cleared
      if (st_lookup_excluding_parent(top->scope, t->attr.name) != -1)
      {
        fprintf(listing, "Error: Symbol \"%s\" is redefined at line %d\n", t->attr.name, t->lineno);
      }
      else
      {
        st_insert(top->scope, t->attr.name, t->type, t->lineno, t->attr.val);
        char *name = t->attr.name;
        push(name, t->lineno);

        // make_table(top->scope,t->attr.name);
        // push(t->attr.name);
      }

      break;
    default:
      break;
    }
    break;
  case ParamK: // cleared
    switch (t->kind.prm)
    {
    case NArrK: // cleared
      if (st_lookup_excluding_parent(top->scope, t->attr.name) != -1)
        /* not yet in table, so treat as new definition */
        fprintf(listing, "Error: Symbol \"%s\" is redefined at line %d\n", t->attr.name, t->lineno);
      else
        /* already in table, so ignore location,
           add line number of use only */
        st_insert(top->scope, t->attr.name, t->type, t->lineno, location++);
      break;
    case ArrPK: // cleared
      if (st_lookup_excluding_parent(top->scope, t->attr.name) != -1)
        /* not yet in table, so treat as new definition */
        fprintf(listing, "Error: Symbol \"%s\" is redefined at line %d\n", t->attr.name, t->lineno);
      else
        /* already in table, so ignore location,
           add line number of use only */
        st_insert(top->scope, t->attr.name, t->type, t->lineno, location++);
      break;
    case NullK:
      break;
    default:
      break;
    }
    break;
  default:

    break;
  }
}

/* Function buildSymtab constructs the symbol
 * table by preorder traversal of the syntax tree
 */
// need to MODIFY
void buildSymtab(TreeNode *syntaxTree)
{
  make_header();
  traverse(syntaxTree, insertNode, nullProc);
  if (TraceAnalyze)
  {
    fprintf(listing, "\nSymbol table:\n\n");
    printSymTab(listing);
  }
}

static void typeError(TreeNode *t, char *message)
{
  fprintf(listing, "Error: The void-type variable is declared at line %d (name : \"%s\")\n", t->lineno, t->attr.name);
  // fprintf(listing,"Type error at line %d: %s\n",t->lineno,message);
  Error = TRUE;
  // Error = FALSE;
}

/* Procedure checkNode performs
 * type checking at a single tree node
 */
// need to MODIFY
static void checkNode(TreeNode *t)
{
  switch (t->nodekind)
  {
  case StmtK:
    switch (t->kind.stmt)
    {
    case IfK:
      if (t->child[0]->type != Integer)
      {
        fprintf(listing, "Error: Invalid condition at line %d\n", t->lineno);
      }
      break;
    case ElseK:
      if (t->child[0]->type != Integer)
      {
        fprintf(listing, "Error: Invalid condition at line %d\n", t->lineno);
      }
      break;
    case WhileK:
      if (t->child[0]->type != Integer)
      {
        fprintf(listing, "Error: Invalid condition at line %d\n", t->lineno);
      }
      break;
    case ReturnK:
    { // Difficult
      StackPtr tmp = top;
      while (strcmp(tmp->parent->scope, "global") != 0)
      {
        tmp = tmp->parent;
      }
      char *name = tmp->scope;
      int Type = st_lookup_excluding_parent("global", name);
      if (Type != t->type)
      {
        fprintf(listing, "Error: Invalid return at line %d\n", t->lineno);
      }
      break;
    }
    case NonReturnK: // Difficult
    {                // Difficult
      StackPtr tmp = top;
      while (strcmp(tmp->parent->scope, "global") != 0)
      {
        tmp = tmp->parent;
      }
      char *name = tmp->scope;
      int Type = st_lookup_excluding_parent("global", name);
      if (Type != t->type)
      {
        fprintf(listing, "Error: Invalid return at line %d\n", t->lineno);
      }
      break;
    }
    case CompK:
      break;
    case AssignK:
      if (t->child[0]->type != Integer || t->child[1]->type != Integer)
      {
        fprintf(listing, "Error: Invalid assignment at line %d\n", t->lineno);
      }
      else
        t->type = Integer;
      break;
    default:
      break;
    }
    break;
  case ExpK:
    switch (t->kind.exp)
    {
      int Type = st_lookup(top->scope, t->attr.name);

    case OpK:
      if (t->child[0]->type != Integer || t->child[1]->type != Integer)
      {
        fprintf(listing, "Error: Invalid assignment at line %d\n", t->lineno);
      }
      else
        t->type = Integer;
      break;
    case ConstK:
      break;
    case IdK:
      if (Type == -1)
      {
        int numparam = getnumparam(t->attr.name);
        if (numparam >= 0)
        {
          fprintf(listing, "Error: Undeclared function \"%s\" is called at line %d\n", t->attr.name, t->lineno);
        }
        else
        {
          fprintf(listing, "Error: Undeclared variable \"%s\" is used at line %d\n", t->attr.name, t->lineno);
        }
      }
      else
      {
        t->type = Type;
      }

      break;
    case ArrEK:
      if (Type == -1)
        fprintf(listing, "Error: Undeclared variable \"%s\" is used at line %d\n", t->attr.name, t->lineno);
      else if (t->child[0]->type != Integer)
      {
        fprintf(listing, "Error: Invalid array indexing at line %d (name : \"%s\"). Indices should be integer\n", lineno, t->attr.name);
      }
      else if (Type != IntArr)
      {
        fprintf(listing, "Error: Invalid array indexing at line %d (name : \"%s\"). Indexing can only be allowed for int[] variables\n", t->lineno, t->attr.name);
      }
      else
        t->type = Type;

      break;
    case CallK:

      if (st_lookup_excluding_parent("global", t->attr.name) == -1)
      {
        fprintf(listing, "Error: Undeclared function \"%s\" is used at line %d\n", t->attr.name, t->lineno);
      }
      else if (t->child[0] == NULL)
      { // 파라미터없이 함수 콜
        if (getnumparam(t->attr.name) != 0)
        {
          fprintf(listing, "Error: Invalid function call at line %d (name : \"%s\")\n", t->lineno, t->attr.name);
        }
      }
      else
      {
        TreeNode *tmp = t->child[0];
        while (tmp != NULL)
        {
          if (st_lookup_excluding_parent(t, tmp->attr.name) == -1)
          {
            fprintf(listing, "Error: Invalid function call at line %d (name : \"%s\")\n", t->lineno, t->attr.name);
            break;
          }
          tmp = tmp->sibling;
        }
      }
      break;
    default:
      break;
    }
    break;
  case DclrK: // cleared
    switch (t->kind.dclr)
    {
    case VarK: // cleared
      break;
    case ArrK: // cleared
      break;
    case FuncK: // cleared
      break;
    default:
      break;
    }
    break;
  case ParamK: // cleared
    switch (t->kind.prm)
    {
    case NArrK: // cleared
      break;
    case ArrPK: // cleared
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

/* Procedure typeCheck performs type checking
 * by a postorder syntax tree traversal
 */
// need to MODIFY
void typeCheck(TreeNode *syntaxTree)
{
  traverse(syntaxTree, nullProc, checkNode);
}
