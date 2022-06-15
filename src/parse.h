#ifndef PARSE_H_
#define PARSE_H_

#include "lex.h"

//Abstract Syntax Tree data types
typedef enum AST_TYPE {PROGRAM, FUNCTION, STATEMENT, EXPRESSION, FUNCNAME} AST_TYPE;

typedef union fields {
    int intVal;
    char *strVal;
    struct children{
      struct astnode_t *left;
      struct astnode_t *right;
    } children;
} fields;

typedef struct astnode_t {
  AST_TYPE nodeType;
  fields fields;
} astnode_t;

//Parsing functions
astnode_t *parseExpression(tokenlist_t *tokens);
astnode_t *parseStatement(tokenlist_t *tokens);
astnode_t *parseFunction(tokenlist_t *tokens);
astnode_t *parseProgram(tokenlist_t *tokens);

//AST printing functions
void printASTNodeType(astnode_t *node);
void printAST(astnode_t *root);

#endif // PARSE_H_
