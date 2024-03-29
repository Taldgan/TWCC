#ifndef PARSE_H_
#define PARSE_H_

#include "lex.h"

//Abstract Syntax Tree data types
typedef enum AST_TYPE {PROGRAM, FUNCTION, STATEMENT, EXPRESSION,
                       DATA, INTEGER, UNARY_OP, BINARY_OP, TERM} AST_TYPE;

typedef union fields {
    int intVal;
    char *strVal;
    struct children{
      struct astnode_t *left;
      struct astnode_t *middle;
      struct astnode_t *right;
    } children;
} fields;

typedef struct astnode_t {
  AST_TYPE nodeType;
  fields fields;
} astnode_t;

//Parsing functions
astnode_t *parseLogicalOrExp(tokenlist_t *tokens);
astnode_t *parseBitXorExpr(tokenlist_t *tokens);
astnode_t *parseBitOrExpr(tokenlist_t *tokens);
astnode_t *parseBitAndExpr(tokenlist_t *tokens);
astnode_t *parseShiftExpr(tokenlist_t *tokens);
astnode_t *parseLogicalAndExp(tokenlist_t *tokens);
astnode_t *parseEqualityExp(tokenlist_t *tokens);
astnode_t *parseRelationalExp(tokenlist_t *tokens);
astnode_t *parseAdditiveExp(tokenlist_t *tokens);
astnode_t *parseTerm(tokenlist_t *tokens);
astnode_t *parseFactor(tokenlist_t *tokens);
astnode_t *parseExpression(tokenlist_t *tokens);
astnode_t *parseStatement(tokenlist_t *tokens);
astnode_t *parseFunction(tokenlist_t *tokens);
astnode_t *parseProgram(tokenlist_t *tokens);

//AST printing functions
void printASTNodeType(astnode_t *node);
void printAST(astnode_t *root);

#endif // PARSE_H_
