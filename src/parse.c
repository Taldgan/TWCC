#include "parse.h"

#include <stdio.h>
#include <stdlib.h>


/**
 * Backus Naur Grammar:
 *
 * <program> ::= <function>
 * <function> ::= "int" <id> "(" ")" "{" { <statement> } "}"
 * <statement> ::= "return" <exp> ";" | "int" <id> "=" [ <exp> ] ";" | <exp> ";"
 * <exp> ::= <id> "=" <exp> | <logical-or-exp>
 * <logical-or-exp> ::= <logical-and-exp> { "&&" <logical-and-exp> }
 * <logical-and-exp> ::= <equality-exp> { "&&" <equality-exp> }
 * <equality-exp> ::= <relational-exp> { ("!=" | "==") <relational-exp> }
 * <relational-exp> ::= <bit-or-expr> { ("<" | ">" | "<=" | ">=") <bit-or-exp> }
 * <bit-or-expr> ::= <bit-xor-expr> { "^" <bit-xor-expr }
 * <bit-xor-expr> ::= <bit-and-expr> { "|" <bit-and-expr }
 * <bit-and-expr> ::= <shift-expr> { "&" <shift-expr }
 * <shift-expr> ::= <additive-expr> { ("<<" | ">>") <additive-expr}
 * <additive-exp> ::= <term> { ("+" | "-") <term> }
 * <term> ::= <factor> { ("*" | "/" | "%") <factor> }
 * <factor> ::= "(" <exp> ")" | <unary_op> <factor> | <int>
 * <unary_op> ::= "!" | "~" | "-"
 **/
/**
 * parseLogicalAndExp(tokenlist_t *tokens)
 * Parses an equality expression, returning an expression-type AST node
 *
 * <logical-and-expr> ::= <equality-expr> { "&&" <equality-expr> }
 *
 * param *tokens - the token list to parse the expression from
 * return astnode_t* - returns an expression AST node
 **/
astnode_t *parseLogicalOrExp(tokenlist_t *tokens){
  if(tokens == NULL){
    fprintf(stderr, "Cannot parse expression, null token list.\n");
    exit(1);
  }
  token_t *currToken = NULL;
  astnode_t *exprNode = NULL;
  exprNode = parseLogicalAndExp(tokens);
  currToken = peek(tokens);
  //Found first factor, now check for additional mult/division
  while(currToken->type == AND_OP){
    currToken = popToken(tokens);
    char *opVal = currToken->value;
    astnode_t *leftOperand = exprNode;
    astnode_t *operator = (astnode_t *) malloc(sizeof(astnode_t)*1);
    if(operator == NULL){
      fprintf(stderr, "Error on line %d: Failed to allocate space for operator node in term.\n", currToken->lineNum);
      exit(1);
    }
    operator->nodeType = DATA;
    operator->fields.strVal = opVal;
    astnode_t *rightOperand = parseLogicalAndExp(tokens);
    exprNode = (astnode_t *) malloc(sizeof(astnode_t)*1);
    if(exprNode == NULL){
      fprintf(stderr, "Error on line %d: Failed to allocate space for binary operator term node.\n", currToken->lineNum);
      exit(1);
    }
    exprNode->nodeType = BINARY_OP;
    exprNode->fields.children.left = leftOperand;
    exprNode->fields.children.middle = operator;
    exprNode->fields.children.right = rightOperand;
    currToken = peek(tokens);
  }
  return exprNode;
}

/**
 * parseBitOrExpr(tokenlist_t *tokens)
 * Parses a bitwise-xor expression, returning an expression-type AST node
 *
 * <bit-or-expr> ::= <bit-xor-expr> { "^" <bit-xor-expr }
 *
 * param *tokens - the token list to parse the expression from
 * return astnode_t* - returns an expression AST node
 **/
astnode_t *parseBitXorExpr(tokenlist_t *tokens){
  if(tokens == NULL){
    fprintf(stderr, "Cannot parse expression, null token list.\n");
    exit(1);
  }
  token_t *currToken = NULL;
  astnode_t *exprNode = NULL;
  exprNode = parseBitAndExpr(tokens);
  currToken = peek(tokens);
  //Found first factor, now check for additional mult/division
  while(currToken->type == BIT_XOR){
    currToken = popToken(tokens);
    char *opVal = currToken->value;
    astnode_t *leftOperand = exprNode;
    astnode_t *operator = (astnode_t *) malloc(sizeof(astnode_t)*1);
    if(operator == NULL){
      fprintf(stderr, "Error on Line %d: Failed to allocate space for operator node in term.\n", currToken->lineNum);
      exit(1);
    }
    operator->nodeType = DATA;
    operator->fields.strVal = opVal;
    astnode_t *rightOperand = parseBitAndExpr(tokens);
    exprNode = (astnode_t *) malloc(sizeof(astnode_t)*1);
    if(exprNode == NULL){
      fprintf(stderr, "Error on line %d: Failed to allocate space for binary operator term node.\n", currToken->lineNum);
      exit(1);
    }
    exprNode->nodeType = BINARY_OP;
    exprNode->fields.children.left = leftOperand;
    exprNode->fields.children.middle = operator;
    exprNode->fields.children.right = rightOperand;
    currToken = peek(tokens);
  }
  return exprNode;
}

/**
 * parseBitOrExpr(tokenlist_t *tokens)
 * Parses a bitwise-or expression, returning an expression-type AST node
 *
 * <bit-or-expr> ::= <bit-xor-expr> { "^" <bit-xor-expr }
 *
 * param *tokens - the token list to parse the expression from
 * return astnode_t* - returns an expression AST node
 **/
astnode_t *parseBitOrExpr(tokenlist_t *tokens){
  if(tokens == NULL){
    fprintf(stderr, "Cannot parse expression, null token list.\n");
    exit(1);
  }
  token_t *currToken = NULL;
  astnode_t *exprNode = NULL;
  exprNode = parseBitXorExpr(tokens);
  currToken = peek(tokens);
  //Found first factor, now check for additional mult/division
  while(currToken->type == BIT_OR){
    currToken = popToken(tokens);
    char *opVal = currToken->value;
    astnode_t *leftOperand = exprNode;
    astnode_t *operator = (astnode_t *) malloc(sizeof(astnode_t)*1);
    if(operator == NULL){
      fprintf(stderr, "Error on line %d: Failed to allocate space for operator node in term.\n", currToken->lineNum);
      exit(1);
    }
    operator->nodeType = DATA;
    operator->fields.strVal = opVal;
    astnode_t *rightOperand = parseBitXorExpr(tokens);
    exprNode = (astnode_t *) malloc(sizeof(astnode_t)*1);
    if(exprNode == NULL){
      fprintf(stderr, "Error on line %d: Failed to allocate space for binary operator term node.\n", currToken->lineNum);
      exit(1);
    }
    exprNode->nodeType = BINARY_OP;
    exprNode->fields.children.left = leftOperand;
    exprNode->fields.children.middle = operator;
    exprNode->fields.children.right = rightOperand;
    currToken = peek(tokens);
  }
  return exprNode;
}

/**
 * parseBitAndExpr(tokenlist_t *tokens)
 * Parses a bitwise-and expression, returning an expression-type AST node
 *
 * <bit-and-expr> ::= <shift-expr> { "&" <shift-expr }
 *
 * param *tokens - the token list to parse the expression from
 * return astnode_t* - returns an expression AST node
 **/
astnode_t *parseBitAndExpr(tokenlist_t *tokens){
  if(tokens == NULL){
    fprintf(stderr, "Cannot parse expression, null token list.\n");
    exit(1);
  }
  token_t *currToken = NULL;
  astnode_t *exprNode = NULL;
  exprNode = parseShiftExpr(tokens);
  currToken = peek(tokens);
  //Found first factor, now check for additional mult/division
  while(currToken->type == BIT_AND){
    currToken = popToken(tokens);
    char *opVal = currToken->value;
    astnode_t *leftOperand = exprNode;
    astnode_t *operator = (astnode_t *) malloc(sizeof(astnode_t)*1);
    if(operator == NULL){
      fprintf(stderr, "Error on line %d: Failed to allocate space for operator node in term.\n", currToken->lineNum);
      exit(1);
    }
    operator->nodeType = DATA;
    operator->fields.strVal = opVal;
    astnode_t *rightOperand = parseShiftExpr(tokens);
    exprNode = (astnode_t *) malloc(sizeof(astnode_t)*1);
    if(exprNode == NULL){
      fprintf(stderr, "Error on line %d: Failed to allocate space for binary operator term node.\n", currToken->lineNum);
      exit(1);
    }
    exprNode->nodeType = BINARY_OP;
    exprNode->fields.children.left = leftOperand;
    exprNode->fields.children.middle = operator;
    exprNode->fields.children.right = rightOperand;
    currToken = peek(tokens);
  }
  return exprNode;
}

/**
 * parseLogicalAndExp(tokenlist_t *tokens)
 * Parses an equality expression, returning an expression-type AST node
 *
 * <logical-and-expr> ::= <equality-expr> { "&&" <equality-expr> }
 *
 * param *tokens - the token list to parse the expression from
 * return astnode_t* - returns an expression AST node
 **/
astnode_t *parseLogicalAndExp(tokenlist_t *tokens){
  if(tokens == NULL){
    fprintf(stderr, "Cannot parse expression, null token list.\n");
    exit(1);
  }
  token_t *currToken = NULL;
  astnode_t *exprNode = NULL;
  exprNode = parseBitOrExpr(tokens);
  currToken = peek(tokens);
  //Found first factor, now check for additional mult/division
  while(currToken->type == AND_OP){
    currToken = popToken(tokens);
    char *opVal = currToken->value;
    astnode_t *leftOperand = exprNode;
    astnode_t *operator = (astnode_t *) malloc(sizeof(astnode_t)*1);
    if(operator == NULL){
      fprintf(stderr, "Error on line %d: Failed to allocate space for operator node in term.\n", currToken->lineNum);
      exit(1);
    }
    operator->nodeType = DATA;
    operator->fields.strVal = opVal;
    astnode_t *rightOperand = parseBitOrExpr(tokens);
    exprNode = (astnode_t *) malloc(sizeof(astnode_t)*1);
    if(exprNode == NULL){
      fprintf(stderr, "Error on line %d: Failed to allocate space for binary operator term node.\n", currToken->lineNum);
      exit(1);
    }
    exprNode->nodeType = BINARY_OP;
    exprNode->fields.children.left = leftOperand;
    exprNode->fields.children.middle = operator;
    exprNode->fields.children.right = rightOperand;
    currToken = peek(tokens);
  }
  return exprNode;
}

/**
 * parseEqualityExp(tokenlist_t *tokens)
 * Parses an equality expression, returning an expression-type AST node
 *
 * <equality-expr> ::= <relational-expr> { ("!=" | "==" ) <relational-expr> }
 *
 * param *tokens - the token list to parse the expression from
 * return astnode_t* - returns an expression AST node
 **/
astnode_t *parseEqualityExp(tokenlist_t *tokens){
  if(tokens == NULL){
    fprintf(stderr, "Cannot parse expression, null token list.\n");
    exit(1);
  }
  token_t *currToken = NULL;
  astnode_t *exprNode = NULL;
  exprNode = parseRelationalExp(tokens);
  currToken = peek(tokens);
  //Found first factor, now check for additional mult/division
  while(currToken->type == NEQ_TO || currToken->type == EQ_TO){
    currToken = popToken(tokens);
    char *opVal = currToken->value;
    astnode_t *leftOperand = exprNode;
    astnode_t *operator = (astnode_t *) malloc(sizeof(astnode_t)*1);
    if(operator == NULL){
      fprintf(stderr, "Error on line %d: Failed to allocate space for operator node in term.\n", currToken->lineNum);
      exit(1);
    }
    operator->nodeType = DATA;
    operator->fields.strVal = opVal;
    astnode_t *rightOperand = parseRelationalExp(tokens);
    exprNode = (astnode_t *) malloc(sizeof(astnode_t)*1);
    if(exprNode == NULL){
      fprintf(stderr, "Error on line %d: Failed to allocate space for binary operator term node.\n", currToken->lineNum);
      exit(1);
    }
    exprNode->nodeType = BINARY_OP;
    exprNode->fields.children.left = leftOperand;
    exprNode->fields.children.middle = operator;
    exprNode->fields.children.right = rightOperand;
    currToken = peek(tokens);
  }
  return exprNode;
}

/**
 * parseShiftExpr(tokenlist_t *tokens)
 * Parses a bit-shift expression, returning an expression-type AST node
 *
 * <relational-expr> ::= <additive-expr> { ("<" | ">" | "<=" | ">=") <additive-expr> }
 *
 * param *tokens - the token list to parse the expression from
 * return astnode_t* - returns an expression AST node
 **/
astnode_t *parseShiftExpr(tokenlist_t *tokens){
  if(tokens == NULL){
    fprintf(stderr, "Cannot parse expression, null token list.\n");
    exit(1);
  }
  token_t *currToken = NULL;
  astnode_t *exprNode = NULL;
  exprNode = parseAdditiveExp(tokens);
  currToken = peek(tokens);
  //Found first factor, now check for additional mult/division
  while(currToken->type == LT_OP || currToken->type == GT_OP || currToken->type == LE_OP || currToken->type == GE_OP){
    currToken = popToken(tokens);
    char *opVal = currToken->value;
    astnode_t *leftOperand = exprNode;
    astnode_t *operator = (astnode_t *) malloc(sizeof(astnode_t)*1);
    if(operator == NULL){
      fprintf(stderr, "Error on line %d: Failed to allocate space for operator node in term.\n", currToken->lineNum);
      exit(1);
    }
    operator->nodeType = DATA;
    operator->fields.strVal = opVal;
    astnode_t *rightOperand = parseAdditiveExp(tokens);
    exprNode = (astnode_t *) malloc(sizeof(astnode_t)*1);
    if(exprNode == NULL){
      fprintf(stderr, "Error on line %d: Failed to allocate space for binary operator term node.\n", currToken->lineNum);
      exit(1);
    }
    exprNode->nodeType = BINARY_OP;
    exprNode->fields.children.left = leftOperand;
    exprNode->fields.children.middle = operator;
    exprNode->fields.children.right = rightOperand;
    currToken = peek(tokens);
  }
  return exprNode;
}

/**
 * parseRelationalExp(tokenlist_t *tokens)
 * Parses a relational expression, returning an expression-type AST node
 *
 * <relational-expr> ::= <additive-expr> { ("<" | ">" | "<=" | ">=") <additive-expr> }
 *
 * param *tokens - the token list to parse the expression from
 * return astnode_t* - returns an expression AST node
 **/
astnode_t *parseRelationalExp(tokenlist_t *tokens){
  if(tokens == NULL){
    fprintf(stderr, "Cannot parse expression, null token list.\n");
    exit(1);
  }
  token_t *currToken = NULL;
  astnode_t *exprNode = NULL;
  exprNode = parseShiftExpr(tokens);
  currToken = peek(tokens);
  //Found first factor, now check for additional mult/division
  while(currToken->type == LT_OP || currToken->type == GT_OP || currToken->type == LE_OP || currToken->type == GE_OP){
    currToken = popToken(tokens);
    char *opVal = currToken->value;
    astnode_t *leftOperand = exprNode;
    astnode_t *operator = (astnode_t *) malloc(sizeof(astnode_t)*1);
    if(operator == NULL){
      fprintf(stderr, "Error on line %d: Failed to allocate space for operator node in term.\n", currToken->lineNum);
      exit(1);
    }
    operator->nodeType = DATA;
    operator->fields.strVal = opVal;
    astnode_t *rightOperand = parseShiftExpr(tokens);
    exprNode = (astnode_t *) malloc(sizeof(astnode_t)*1);
    if(exprNode == NULL){
      fprintf(stderr, "Error on line %d: Failed to allocate space for binary operator term node.\n", currToken->lineNum);
      exit(1);
    }
    exprNode->nodeType = BINARY_OP;
    exprNode->fields.children.left = leftOperand;
    exprNode->fields.children.middle = operator;
    exprNode->fields.children.right = rightOperand;
    currToken = peek(tokens);
  }
  return exprNode;
}

/**
 * parseAdditiveExp(tokenlist_t *tokens)
 * Parses an additive expression, returning an expression-type AST node
 *
 * <additive-expr> ::= <term> { ( "+" | - ) <term> }
 *
 * param *tokens - the token list to parse the expression from
 * return astnode_t* - returns an expression AST node
 **/
astnode_t *parseAdditiveExp(tokenlist_t *tokens){
  if(tokens == NULL){
    fprintf(stderr, "Cannot parse expression, null token list.\n");
    exit(1);
  }
  token_t *currToken = NULL;
  astnode_t *exprNode = NULL;
  exprNode = parseTerm(tokens);
  currToken = peek(tokens);
  //Found first factor, now check for additional mult/division
  while(currToken->type == ADD_OP || currToken->type == NEGATION){
    currToken = popToken(tokens);
    char *opVal = currToken->value;
    astnode_t *leftOperand = exprNode;
    astnode_t *operator = (astnode_t *) malloc(sizeof(astnode_t)*1);
    if(operator == NULL){
      fprintf(stderr, "Error on line %d: Failed to allocate space for operator node in term.\n", currToken->lineNum);
      exit(1);
    }
    operator->nodeType = DATA;
    operator->fields.strVal = opVal;
    astnode_t *rightOperand = parseTerm(tokens);
    exprNode = (astnode_t *) malloc(sizeof(astnode_t)*1);
    if(exprNode == NULL){
      fprintf(stderr, "Error on line %d: Failed to allocate space for binary operator term node.\n", currToken->lineNum);
      exit(1);
    }
    exprNode->nodeType = BINARY_OP;
    exprNode->fields.children.left = leftOperand;
    exprNode->fields.children.middle = operator;
    exprNode->fields.children.right = rightOperand;
    currToken = peek(tokens);
  }
  return exprNode;
}

/**
 * parseTerm(tokenlist_t *tokens)
 * Parse a term, returning a term-type AST node
 *
 * <term> ::= <factor> { ("*" | "/" | "%") <factor> }
 *
 * return astnode_t* - returns the created AST node
 **/
astnode_t *parseTerm(tokenlist_t *tokens){
  if(tokens == NULL){
    fprintf(stderr, "Cannot parse term, null token list.\n");
    exit(1);
  }
  token_t *currToken = NULL;
  astnode_t *termNode = NULL;
  termNode = parseFactor(tokens);
  currToken = peek(tokens);
  //Found first factor, now check for additional mult/division
  while(currToken->type == MULT_OP || currToken->type == DIV_OP || currToken->type == MOD_OP){
    currToken = popToken(tokens);
    char *opVal = currToken->value;
    astnode_t *leftOperand = termNode;
    astnode_t *operator = (astnode_t *) malloc(sizeof(astnode_t)*1);
    if(operator == NULL){
      fprintf(stderr, "Error on line %d: Failed to allocate space for operator node in term.\n", currToken->lineNum);
      exit(1);
    }
    operator->nodeType = DATA;
    operator->fields.strVal = opVal;
    astnode_t *rightOperand = parseFactor(tokens);
    termNode = (astnode_t *) malloc(sizeof(astnode_t)*1);
    if(termNode == NULL){
      fprintf(stderr, "Error on line %d: Failed to allocate space for binary operator term node.\n", currToken->lineNum);
      exit(1);
    }
    termNode->nodeType = BINARY_OP;
    termNode->fields.children.left = leftOperand;
    termNode->fields.children.middle = operator;
    termNode->fields.children.right = rightOperand;
    currToken = peek(tokens);
  }
  return termNode;
}

/**
 * parseFactor(tokenlist_t *tokens)
 * Parse a factor, returning a factor-type AST node
 *
 * <factor> ::= "(" <expression> ")" | <un_op> <factor> | <int>
 *
 * return astnode_t* - returns the created AST node
 **/
astnode_t *parseFactor(tokenlist_t *tokens){
  if(tokens == NULL){
    fprintf(stderr, "Cannot parse factor, null token list.\n");
    exit(1);
  }
  token_t *currToken = NULL;
  astnode_t *factNode = NULL;
  currToken = popToken(tokens);
  if(currToken == NULL){
    fprintf(stderr, "Error on line %d: Empty factor, invalid format.\n", currToken->lineNum);
    exit(1);
  }
  if(currToken->type == OPEN_PAREN){
    factNode = parseExpression(tokens);
    currToken = popToken(tokens);
    if(currToken->type != CLOSED_PAREN){
      fprintf(stderr, "Error on line %d: Missing closed parenthese in factor.\n", currToken->lineNum);
      exit(1);
    }
    return factNode;
  }
  //Need to malloc factNode for remaining cases
  factNode = (astnode_t *) malloc(sizeof(astnode_t)*1);
  if(factNode == NULL){
    fprintf(stderr, "Error on line %d: Failed to allocate space for int factor node.\n", currToken->lineNum);
    exit(1);
  }
  else if(currToken->type == NEGATION || currToken->type == BITWISE_COMP || currToken->type == LOGIC_NEG){
    astnode_t *unOp = (astnode_t*) malloc(sizeof(astnode_t)*1);
    if(unOp == NULL){
      fprintf(stderr, "Error on line %d: Failed to allocate space for unary operator node.\n", currToken->lineNum);
      exit(1);
    }
    unOp->fields.strVal = currToken->value;
    unOp->nodeType = DATA;
    factNode->nodeType = UNARY_OP;
    factNode->fields.children.left = unOp;
    factNode->fields.children.right = parseFactor(tokens);
    return factNode;
  }
  else if(currToken->type == INT_LITERAL){
    factNode->nodeType = INTEGER;
    factNode->fields.intVal = atoi(currToken->value);
    return factNode;
  }
  else{
    fprintf(stderr, "Error on line %d: Invalid factor.\n", currToken->lineNum);
    exit(1);
  }
  return NULL;
}

/**
 * parseExpression(tokenlist_t *tokens)
 * Parses an expression, returning an expression-type AST node
 *
 * <expression> ::= <logical-and-expr> { "||" < logical-and-expr> }
 *
 * param *tokens - the token list to parse the expression from
 * return astnode_t* - returns an expression AST node
 **/
astnode_t *parseExpression(tokenlist_t *tokens){
  if(tokens == NULL){
    fprintf(stderr, "Cannot parse expression, null token list.\n");
    exit(1);
  }
  token_t *currToken = NULL;
  astnode_t *exprNode = NULL;
  exprNode = parseLogicalOrExp(tokens);
  currToken = peek(tokens);
  //Found first factor, now check for additional mult/division
  while(currToken->type == OR_OP){
    currToken = popToken(tokens);
    char *opVal = currToken->value;
    astnode_t *leftOperand = exprNode;
    astnode_t *operator = (astnode_t *) malloc(sizeof(astnode_t)*1);
    if(operator == NULL){
      fprintf(stderr, "Error on line %d: Failed to allocate space for operator node in term.\n", currToken->lineNum);
      exit(1);
    }
    operator->nodeType = DATA;
    operator->fields.strVal = opVal;
    astnode_t *rightOperand = parseLogicalOrExp(tokens);
    exprNode = (astnode_t *) malloc(sizeof(astnode_t)*1);
    if(exprNode == NULL){
      fprintf(stderr, "Error on line %d: Failed to allocate space for binary operator term node.\n", currToken->lineNum);
      exit(1);
    }
    exprNode->nodeType = BINARY_OP;
    exprNode->fields.children.left = leftOperand;
    exprNode->fields.children.middle = operator;
    exprNode->fields.children.right = rightOperand;
    currToken = peek(tokens);
  }
  return exprNode;
}

/**
 * parseStatement(tokenlist_t *tokens)
 * Parses a statement, returning a statement-type AST node
 *
 * <statement> ::= "return" <expression> ";"
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
    fprintf(stderr, "Error on line %d: Statement did not begin with return.\n", currToken->lineNum);
    exit(1);
  }
  statementNode = (astnode_t *) malloc(sizeof(astnode_t)*1);
  statementNode->nodeType = STATEMENT;
  statementNode->fields.children.left = parseExpression(tokens);
  currToken = popToken(tokens);
  if(currToken == NULL || currToken->type != SEMICOLON){
    fprintf(stderr, "Error on line %d: Statement did not end with semicolon.\n", currToken->lineNum);
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
    fprintf(stderr, "Error on line %d: Function did not begin with int keyword.\n", currToken->lineNum);
    exit(1);
  }
  currToken = popToken(tokens);
  if(currToken == NULL || currToken->type != IDENTIFIER){
    fprintf(stderr, "Error on line %d: Identifier did not follow int keyword.\n", currToken->lineNum);
    exit(1);
  }
  funcName = currToken->value;
  funcNode = (astnode_t *) malloc(sizeof(astnode_t)*1);
  funcNode->nodeType = FUNCTION;
  funcNode->fields.children.left = (astnode_t *) malloc(sizeof(astnode_t)*1);
  funcNode->fields.children.left->nodeType = DATA;
  if(funcNode->fields.children.left == NULL){
    fprintf(stderr, "Error on line %d: Failed to allocate space for function name node.\n", currToken->lineNum);
    exit(1);
  }
  //Function left child node will contain value of function's name, right func body
  funcNode->fields.children.left->fields.strVal = funcName;
  currToken = popToken(tokens);
  if(currToken == NULL || currToken->type != OPEN_PAREN){
    fprintf(stderr, "Error on line %d: Open parenthese did not follow identifier.\n", currToken->lineNum);
    exit(1);
  }
  currToken = popToken(tokens);
  if(currToken == NULL || currToken->type != CLOSED_PAREN){
    fprintf(stderr, "Error on line %d: Closed parenthese did not follow open parenthese.\n", currToken->lineNum);
    exit(1);
  }
  currToken = popToken(tokens);
  if(currToken == NULL || currToken->type != OPEN_BRACE){
    fprintf(stderr, "Error on line %d: Open bracket did not follow closed parenthese.\n", currToken->lineNum);
    exit(1);
  }
  //Create func body
  funcNode->fields.children.right = parseStatement(tokens);
  currToken = popToken(tokens);
  if(currToken == NULL || currToken->type != CLOSED_BRACE){
    fprintf(stderr, "Error on line %d: Closed bracket missing for function %s.\n", currToken->lineNum, funcName);
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
  case TERM:
    printf("TERM");
    break;
  case DATA:
    printf("DATA");
    break;
  case INTEGER:
    printf("INTEGER");
    break;
  case UNARY_OP:
    printf("UN_OP");
    break;
  case BINARY_OP:
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
  else if(currNode->nodeType == UNARY_OP){
    printf("%s", currNode->fields.children.left->fields.strVal);
    printAST(currNode->fields.children.right);
    return;
  }
  else if(currNode->nodeType == INTEGER){
    printf("%d", currNode->fields.intVal);
    return;
  }
  else if(currNode->nodeType == BINARY_OP){
    printAST(currNode->fields.children.left);
    printf(" %s ", currNode->fields.children.middle->fields.strVal);
    printAST(currNode->fields.children.right);
    return;
  }
  puts("");
}
