#include "parse.h"

#include <stdio.h>
#include <stdlib.h>

/**
 * parseTerm(tokenlist_t *tokens)
 *
 **/

/**
 * parseFactor(tokenlist_t *tokens)
 * Pare a factor, returnaing a factor-type AST node
 *
 * <factor> ::= "(" <expression> ")" | <un_op> <factor> | <int>
 **/
astnode_t *parseFactor(tokenlist_t *tokens){
  token_t *currToken = NULL;
  astnode_t *factNode = NULL;
  currToken = popToken(tokens);
  if(currToken == NULL){
    fprintf(stderr, "No value/operator followed unary operator.\n");
    exit(1);
  }
  factNode = (astnode_t *) malloc(sizeof(astnode_t)*1);
  //next 2 need to be another expression & then closed parentheses
  if(currToken->type == OPEN_PAREN){
    astnode_t *exprNode = NULL;
    exprNode = parseExpression(tokens);
    currToken = popToken(tokens);
    if(currToken->type != CLOSED_PAREN){
      fprintf(stderr, "Missing closed parentheses in factor.\n");
      exit(1);
    }
    factNode->nodeType = EXPRESSION;
    factNode->fields.children.left = exprNode;
    return factNode;
  }
  //Else unop, has to be followed by another factor
  else if(currToken->type == NEGATION || currToken->type == BITWISE_COMP || currToken->type == LOGIC_NEG){
    astnode_t *unOp = (astnode_t*) malloc(sizeof(astnode_t)*1);
    if(unOp == NULL){
      fprintf(stderr, "Failed to allocate space for unary operator node.\n");
      exit(1);
    }
    unOp->fields.strVal = currToken->value;
    unOp->nodeType = DATA;
    factNode->fields.children.left = unOp;
    factNode->fields.children.right = parseFactor(tokens);
    factNode->nodeType = UN_OP;
    return factNode;
  }
  else if(currToken->type == INT_LITERAL){
    //Int, return
    factNode->nodeType = INTEGER;
    factNode->fields.intVal = atoi(currToken->value);
    return factNode;
  }
  else{
    fprintf(stderr, "Unary op has to applied to factor, invalid format.\n");
    exit(1);
  }
  return NULL;
}
/**
 * parseExpression(tokenlist_t *tokens)
 * Parses an expression, returning a expression-type AST node
 *
 * <expression> ::= <expression> <bin_op> <expression> | <factor>
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
  //if binary operation
  if(tokens->head->type == ADD_OP || tokens->head->type == MULT_OP || tokens->head->type == DIV_OP){
    astnode_t *leftOperand = (astnode_t *) malloc(sizeof(astnode_t)*1);
    astnode_t *operator = (astnode_t *) malloc(sizeof(astnode_t)*1);
    astnode_t *rightOperand = (astnode_t *) malloc(sizeof(astnode_t)*1);
    if(leftOperand == NULL || operator == NULL || rightOperand == NULL){
      fprintf(stderr, "Failed to allocate space for binary operator or operands.\n");
      exit(1);
    }
    char *opType = tokens->head->value;
    tokens->head->value = currToken->value;
    tokens->head->type = currToken->type;
    operator->fields.strVal = opType;
    if(currToken == NULL){
      fprintf(stderr, "Missing value after operator %s.\n", operator->fields.strVal);
      exit(1);
    }
    leftOperand = parseExpression(tokens);
    rightOperand = parseExpression(tokens);
    exprNode->nodeType = BIN_OP;
    exprNode->fields.children.left = leftOperand;
    exprNode->fields.children.middle = operator;
    exprNode->fields.children.right = rightOperand;
    return exprNode;
  }
  //else factor?
  else{

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
    case BIN_OP:
      printf("BIN_OP");
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
    printf(";");
    return;
  }
  else if(currNode->nodeType == UN_OP){
    printf("%s", currNode->fields.children.left->fields.strVal);
    printAST(currNode->fields.children.right);
    return;
  }
  else if(currNode->nodeType == INTEGER){
    printf("%d", currNode->fields.intVal);
    return;
  }
  else if(currNode->nodeType == BIN_OP){
    printAST(currNode->fields.children.left);
    printf(" %s ", currNode->fields.children.middle->fields.strVal);
    printAST(currNode->fields.children.right);
    return;
  }
  puts("");
}
