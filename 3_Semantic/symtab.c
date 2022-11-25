/****************************************************/
/* File: symtab.c                                   */
/* Symbol table implementation for the TINY compiler*/
/* (allows only one symbol table)                   */
/* Symbol table is implemented as a chained         */
/* hash table                                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

/* SIZE is the size of the hash table */
#define SIZE 3269

/* SHIFT is the power of two used as multiplier
   in hash function  */
#define SHIFT 4

/* the list of line numbers of the source 
 * code in which a variable is referenced
 */
typedef struct LineListRec
   { int lineno;
     struct LineListRec * next;
   } * LineList;

/* The record in the bucket lists for
 * each variable, including name, 
 * assigned memory location, and
 * the list of line numbers in which
 * it appears in the source code
 */
 typedef struct Param{
    ExpType type;
    struct Param * next;
 } * ParamList;

typedef struct BucketListRec
   { char * name;
     ExpType type;
     LineList lines;
     ParamList params;
     char * scope;
     int cntparam;
     //char * scope;
     int memloc ; /* memory location for variable */
     struct BucketListRec * next;
   } * BucketList;

typedef struct ScopeListRec
{
  char * name;
  BucketList bucket;  // Similar Role of Global Scope
  struct ScopeListRec * parent;
} * ScopeList;

/* the hash table */ // Role of Global Scope
//static BucketList hashTable[SIZE];
static ScopeList Table;

void make_table(char * parScope, char * newScope){
  ScopeList newNode = (ScopeList)malloc(sizeof(struct ScopeListRec));
  newNode -> name = newScope;
  for(int i =0;i<SIZE;i++){
    newNode->bucket[i]=NULL;
  }
  newNode -> parent = parScope;
  int h = hash(newScope);
  hashTable[h] = newNode;
}


void delete_table(char * parScope, char * newScope){
  int h = hash(newScope);
  free(hashTable[h]);
  
}



/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
 void st_insert(char* scope, char * name, ExpType type, int lineno, int loc ){
  int h = hash(scope);
  ScopeList l = hashTable[h]; // a Scope
  int hh = hash(name);
  BucketList b = l->bucket[hh];

  while ((b != NULL) && (strcmp(name,b->name) != 0)) // look same scope
    b = b->next;
  if (b == NULL) /* variable not yet in table */
  { b = (BucketList) malloc(sizeof(struct BucketListRec));
    b->name = name;
    b->lines = (LineList) malloc(sizeof(struct LineListRec));
    b->lines->lineno = lineno;
    b->params = NULL;
    b->cntparam=-1;
    b->memloc = loc;
    b->lines->next = NULL;
    b->next = l->bucket[hh];
    l->bucket[hh] = b; }
  else /* found in table, so just add line number */
  { LineList t = b->lines;
    while (t->next != NULL) t = t->next;
    t->next = (LineList) malloc(sizeof(struct LineListRec));
    t->next->lineno = lineno;
    t->next->next = NULL;
  }
 }
/*
void st_insert( char * name, int lineno, int loc )
{ int h = hash(name);
  BucketList l =  hashTable[h];
  while ((l != NULL) && (strcmp(name,l->name) != 0))
    l = l->next;
  if (l == NULL)
  { l = (BucketList) malloc(sizeof(struct BucketListRec));
    l->name = name;
    l->lines = (LineList) malloc(sizeof(struct LineListRec));
    l->lines->lineno = lineno;
    l->memloc = loc;
    l->lines->next = NULL;
    l->next = hashTable[h];
    hashTable[h] = l; }
  else 
  { LineList t = l->lines;
    while (t->next != NULL) t = t->next;
    t->next = (LineList) malloc(sizeof(struct LineListRec));
    t->next->lineno = lineno;
    t->next->next = NULL;
  }
} 
*/

/* Function st_lookup returns the memory 
 * location of a variable or -1 if not found
 */


int st_lookup ( char* scope, char * name )
 { 

  int h = hash(scope);
  ScopeList l = hashTable[h]; // a Scope
  int hh = hash(name);
  BucketList b = l->bucket[hh];
  while(strcmp(l->name,"global")!=0){
  while ((b != NULL) && (strcmp(name,b->name) != 0))
    b = b->next;
  if (b != NULL) {
    return b->type;
  }
  else {
    l=l->parent;
    b = l->bucket[hh];
    continue;
  }
}
  while ((b != NULL) && (strcmp(name,b->name) != 0))
    b = b->next;
  if (b != NULL) {
    return b->type;
  }
  else {
    return -1;
  }
 }

int st_lookup_excluding_parent ( char* scope, char * name ){
  int h = hash(scope);
  ScopeList l = hashTable[h]; // a Scope
  int hh = hash(name);
  BucketList b = l->bucket[hh];
  while ((b != NULL) && (strcmp(name,b->name) != 0))
    b = b->next;
  if (b != NULL) {

    return b->type;
  }
  else {

    return -1;
}
}
int getnumparam(char * name){
  {
  int h = hash("global");
  ScopeList l = hashTable[h]; // a Scope
  int hh = hash(name);
  BucketList b = l->bucket[hh];

  while ((b != NULL) && (strcmp(name,b->name) != 0)) // look same scope
    b = b->next;
    return b->cntparam;
 }
}
/*
int st_lookup ( char * name )
{ int h = hash(name);
  BucketList l =  hashTable[h];
  while ((l != NULL) && (strcmp(name,l->name) != 0))
    l = l->next;
  if (l == NULL) return -1;
  else return l->memloc;
}
*/
/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE * listing)
{ int i;
  fprintf(listing,"Variable Name  Location   Line Numbers\n");
  fprintf(listing,"-------------  --------   ------------\n");
  for (i=0;i<SIZE;++i)
  { if (hashTable[i] != NULL)
    { BucketList l = hashTable[i];
      while (l != NULL)
      { LineList t = l->lines;
        fprintf(listing,"%-14s ",l->name);
        fprintf(listing,"%-8d  ",l->memloc);
        while (t != NULL)
        { fprintf(listing,"%4d ",t->lineno);
          t = t->next;
        }
        fprintf(listing,"\n");
        l = l->next;
      }
    }
  }
} /* printSymTab */
