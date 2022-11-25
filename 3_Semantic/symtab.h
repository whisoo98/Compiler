/****************************************************/
/* File: symtab.h                                   */
/* Symbol table interface for the TINY compiler     */
/* (allows only one symbol table)                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _SYMTAB_H_
#include "globals.h"
#define _SYMTAB_H_

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
//void st_insert( char * name, int lineno, int loc );
void st_insert(char* scope, char * name, ExpType type, int lineno, int loc );
void make_table(char * parScope, char * newScope);
void delete_table(char * parScope, char * newScope);
/* Function st_lookup returns the memory 
 * location of a variable or -1 if not found
 */
//int st_lookup ( char * name );
int st_lookup ( char* scope, char * name );
int st_lookup_excluding_parent ( char* scope, char * name );
int getnumparam(char * name);
/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE * listing);

#endif
