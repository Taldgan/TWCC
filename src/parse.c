#include "parse.h"

#include <stdio.h>
#include <stdlib.h>

/**
 * parseExpression(tokenlist_t *tokens)
 * Parses an expression, returning a expression-type AST node
 *
 * <expression> ::= CONST | UNARY <expression>
 *
 * param *tokens - the token list to parse the expression from
 * return astnode_t* - returns a expression AST node
 **/
astnode_t *parseExpression(tokenlist_t *tokens){
  token_t *currToken = NULL;
  astnode_t *exprNode = NULL;
  currToken = popToken(tokens);
  if(currToken == NULL || (currToken->type != INT_LITERAL && (currToken->type != NEGATION && currToken->type != BITWISE_COMP && currToken->type != LOGIC_NEG))){
    fprintf(stderr, "No value followed return statement.\n");
    exit(1);
  }
  exprNode = (astnode_t *) malloc(sizeof(astnode_t)*1);
  //Not int literal, must be a unary operator (as of right now)
  if(currToken->type != INT_LITERAL){
    astnode_t *unOp = (astnode_t*) malloc(sizeof(astnode_t)*1);
    if(unOp == NULL){
      fprintf(stderr, "Failed to allocate space for unary operator node.\n");
      exit(1);
    }
    unOp->fields.strVal = currToken->value;
    unOp->nodeType = DATA;
    exprNode->fields.children.left = unOp;
    exprNode->fields.children.right = parseExpression(tokens);
    exprNode->nodeType = UN_OP;
  }
  else{
    exprNode->nodeType = INTEGER;
    exprNode->fields.intVal = atoi(currToken->value);
  }
  if(exprNode == NULL){
    fprintf(stderr, "Failed to allocate space for expression node.\n");
    exit(1);
  }
  return exprNode;
}

/**
 * parseStatement(tokenlist_t *tokens)
 * Parses a statement, returning a statement-type AST node
 *
 * param *tokens - the token list to parse the statement from
 * return astnode_t* - returns a statement AST node
 **/
astnode_t *parseStatement(tokenlist_t *tokens){
  token_t *currToken = NULL;
  astnode_t *statementNode = NULL;
  if(tokens == NULL){
    fprintf(stderr, "Cannot parse statement, null token list.\n");
    exit(1);
  }
  currToken = popToken(tokens);
  if(currToken == NULL || currToken->type != RET_KEYW){
    fprintf(stderr, "Statement did not begin with return.\n");
    exit(1);
  }
  statementNode = (astnode_t *) malloc(sizeof(astnode_t)*1);
  statementNode->nodeType = STATEMENT;
  statementNode->fields.children.left = parseExpression(tokens);
  currToken = popToken(tokens);
  if(currToken == NULL || currToken->type != SEMICOLON){
    fprintf(stderr, "Statement did not end with semicolon.\n");
    exit(1);
  }
  return statementNode;
}

/**
 * parseFunction(tokenlist_t *tokens)
 * Parses a function, returning a function-type AST node
 *
 * param *tokens - the token list to parse the function from
 * return astnode_t* - returns a function AST node
 **/
astnode_t *parseFunction(tokenlist_t *tokens){
  token_t *currToken = NULL;
  currToken = popToken(tokens);
  astnode_t *funcNode = NULL;
  char *funcName = NULL;
  if(currToken == NULL || currToken->type != INT_KEYW){
    fprintf(stderr, "Function did not begin with int keyword.\n");
    exit(1);
  }
  currToken = popToken(tokens);
  if(currToken == NULL || currToken->type != IDENTIFIER){
    fprintf(stderr, "Identifier did not follow int keyword.\n");
    exit(1);
  }
  funcName = currToken->value;
  funcNode = (astnode_t *) malloc(sizeof(astnode_t)*1);
  funcNode->nodeType = FUNCTION;
  funcNode->fields.children.left = (astnode_t *) malloc(sizeof(astnode_t)*1);
  funcNode->fields.children.left->nodeType = DATA;
  if(funcNode->fields.children.left == NULL){
    fprintf(stderr, "Failed to allocate space for function name node.\n");
    exit(1);
  }
  //Function left child node will contain value of function's name, right func body
  funcNode->fields.children.left->fields.strVal = funcName;
  currToken = popToken(tokens);
  if(currToken == NULL || currToken->type != OPEN_PAREN){
    fprintf(stderr, "Open parenthese did not follow identifier.\n");
    exit(1);
  }
  currToken = popToken(tokens);
  if(currToken == NULL || currToken->type != CLOSED_PAREN){
    fprintf(stderr, "Closed parenthese did not follow open parenthese.\n");
    exit(1);
  }
  currToken = popToken(tokens);
  if(currToken == NULL || currToken->type != OPEN_BRACE){
    fprintf(stderr, "Open bracket did not follow closed parenthese.\n");
    exit(1);
  }
  //Create func body
  funcNode->fields.children.right = parseStatement(tokens);
  currToken = popToken(tokens);
  if(currToken == NULL || currToken->type != CLOSED_BRACE){
    fprintf(stderr, "Closed bracket missing for function %s.\n", funcName);
    exit(1);
  }
  return funcNode;
}

/**
 * parseProgram(tokenlist_t *tokens)
 * Parses a program, returning a program-type AST node
 *
 * param *tokens - the token list to parse the program from
 * return astnode_t* - returns a function AST node
 **/
astnode_t* parseProgram(tokenlist_t *tokens){
  //printf("── parsing %s ──\n", sourcePath);
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
  //printf(" - parsing complete -\n\n");
  return root;
}

/**
 * printASTNodeType(astnode_t *node)
 * When provided an AST Node, it prints the nodeType enum as a string
 *
 * param *node - the node to print the nodeType of
 * return void
 **/
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
    case DATA:
      printf("DATA");
      break;
    case INTEGER:
      printf("INTEGER");
      break;
    case UN_OP:
      printf("UN_OP");
      break;
  }
}

/**
 * printAST(astnode_t *root)
 * When provided an AST, it prints its function names & bodies (recursively)
 *
 * param *root - the root node of the AST to print
 * return void
 **/
void printAST(astnode_t *root){
  if(root == NULL)
    return;
  astnode_t *currNode = root;
  if(currNode->nodeType == PROGRAM){
    printAST(currNode->fields.children.left);
  }
  else if(currNode->nodeType == FUNCTION){
    printf("FUNC INT %s\n\tbody:\n", currNode->fields.children.left->fields.strVal);
    printAST(currNode->fields.children.right);
    return;
  }
  else if(currNode->nodeType == STATEMENT){
    printf("\treturn ");
    printAST(currNode->fields.children.left);
    return;
  }
  else if(currNode->nodeType == UN_OP){
    printf("%s", currNode->fields.children.left->fields.strVal);
    printAST(currNode->fields.children.right);
    return;
  }
  else if(currNode->nodeType == INTEGER){
    printf("%d;", currNode->fields.intVal);
    return;
  }
  puts("");
}
