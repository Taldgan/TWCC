#include "parse.h"

#include <stdio.h>
#include <stdlib.h>

astnode_t *parseExpression(tokenlist_t *tokens){
  token_t *currToken = NULL;
  astnode_t *exprNode = NULL;
  currToken = popToken(tokens);
  if(currToken == NULL || currToken->type != INT_LITERAL){
    fprintf(stderr, "Int literal did not follow return statement, invalid format.\n");
    exit(1);
  }
  exprNode = (astnode_t *) malloc(sizeof(astnode_t)*1);
  exprNode->nodeType = EXPRESSION;
  if(exprNode == NULL){
    fprintf(stderr, "Failed to allocate space for expression node.\n");
    exit(1);
  }
  exprNode->fields.intVal = atoi(currToken->value);
  return exprNode;
}

astnode_t *parseStatement(tokenlist_t *tokens){
  token_t *currToken = NULL;
  astnode_t *statementNode = NULL;
  if(tokens == NULL){
    fprintf(stderr, "Cannot parse statement, null token list.\n");
    exit(1);
  }
  currToken = popToken(tokens);
  if(currToken == NULL || currToken->type != RET_KEYW){
    fprintf(stderr, "Statement did not begin with return, invalid format.\n");
    exit(1);
  }
  statementNode = (astnode_t *) malloc(sizeof(astnode_t)*1);
  statementNode->nodeType = STATEMENT;
  statementNode->fields.children.left = parseExpression(tokens);
  currToken = popToken(tokens);
  if(currToken == NULL || currToken->type != SEMICOLON){
    fprintf(stderr, "Statement did not end with semicolon, invalid format.\n");
    exit(1);
  }
  return statementNode;
}

astnode_t *parseFunction(tokenlist_t *tokens){
  token_t *currToken = NULL;
  currToken = popToken(tokens);
  astnode_t *funcNode = NULL;
  char *funcName = NULL;
  if(currToken == NULL || currToken->type != INT_KEYW){
    fprintf(stderr, "Function did not begin with int keyword, invalid format.\n");
    exit(1);
  }
  currToken = popToken(tokens);
  if(currToken == NULL || currToken->type != IDENTIFIER){
    fprintf(stderr, "Identifier did not follow int keyword, invalid format.\n");
    exit(1);
  }
  funcName = currToken->value;
  funcNode = (astnode_t *) malloc(sizeof(astnode_t)*1);
  funcNode->nodeType = FUNCTION;
  funcNode->fields.children.left = (astnode_t *) malloc(sizeof(astnode_t)*1);
  funcNode->fields.children.left->nodeType = FUNCNAME;
  if(funcNode->fields.children.left == NULL){
    fprintf(stderr, "Failed to allocate space for function name node.\n");
    exit(1);
  }
  //Function left child node will contain value of function's name, right func body
  funcNode->fields.children.left->fields.strVal = funcName;
  currToken = popToken(tokens);
  if(currToken == NULL || currToken->type != OPEN_PAREN){
    fprintf(stderr, "Open parenthese did not follow identifier, invalid format.\n");
    exit(1);
  }
  currToken = popToken(tokens);
  if(currToken == NULL || currToken->type != CLOSED_PAREN){
    fprintf(stderr, "Closed parenthese did not follow open parenthese, invalid format.\n");
    exit(1);
  }
  currToken = popToken(tokens);
  if(currToken == NULL || currToken->type != OPEN_BRACE){
    fprintf(stderr, "Open bracket did not follow closed parenthese, invalid format.\n");
    exit(1);
  }
  //Create func body
  funcNode->fields.children.right = parseStatement(tokens);
  currToken = popToken(tokens);
  if(currToken == NULL || currToken->type != CLOSED_BRACE){
    fprintf(stderr, "Closed bracket missing for function %s, invalid format.\n", funcName);
    exit(1);
  }
  return funcNode;
}

astnode_t* parseProgram(tokenlist_t *tokens){
  printf("── parsing %s ──\n", sourcePath);
  astnode_t *root = NULL;
  if(tokens == NULL){
    fprintf(stderr, "Cannot parse program, null token list.\n");
    exit(1);
  }
  root = (astnode_t *) malloc(sizeof(astnode_t)*1);
  if(root == NULL){
    fprintf(stderr, "Failed to allocate space for root AST Node.\n");
    exit(1);
  }
  root->nodeType = PROGRAM;
  root->fields.children.left = parseFunction(tokens);
  printf(" - parsing complete -\n\n");
  return root;
}

void printASTNodeType(astnode_t *node){
  if(node == NULL)
    return;
  AST_TYPE nodeType = node->nodeType;
  switch(nodeType){
    case PROGRAM:
      printf("PROGRAM");
      break;
    case FUNCTION:
      printf("FUNCTION");
      break;
    case STATEMENT:
      printf("STATEMENT");
      break;
    case EXPRESSION:
      printf("EXPRESSION");
      break;
    case FUNCNAME:
      printf("FUNCNAME");
      break;
  }
}

void printAST(astnode_t *root){
  if(root == NULL)
    return;
  astnode_t *currNode = root;
  printASTNodeType(currNode);
  puts("");
  currNode = currNode->fields.children.left;
  printASTNodeType(currNode);
  printf(" INT %s\n", currNode->fields.children.left->fields.strVal);
  currNode = currNode->fields.children.right;
  puts("\tbody:");
  printf("\t\treturn %d\n", currNode->fields.children.left->fields.intVal);
}
