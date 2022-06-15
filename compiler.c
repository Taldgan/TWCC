#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <ctype.h>

/**
 * TODO
 * remember the use of union - which would allow me to assign multiple data types to one AST node
 * Define astnode data type, for use in parse tree
 * Maybe make 'popped' token list? for easy free's?
 **/

/**
 * Custom data types
 **/
#define LEN_PATH 4097
#define NUM_KEYWORDS 9

//Abstract Syntax Tree data types
typedef enum AST_TYPE {PROGRAM, FUNCTION, STATEMENT, EXPRESSION, FUNCNAME} AST_TYPE;

/**
 * For use in AST node data type
 *
 **/
typedef union fields {
    int intVal;
    char *strVal;
    struct children{
      struct astnode_t *left;
      struct astnode_t *right;
    } children;
} fields;

/**
 * AST Node data type.
 * Depending on type of node, union data type is set to either have children (like with operators), or a value
 **/
typedef struct astnode_t {
  AST_TYPE nodeType;
  fields fields;
} astnode_t;

//Token-related data types
typedef enum TOKEN_TYPE {OPEN_BRACE, CLOSED_BRACE, OPEN_PAREN, CLOSED_PAREN, SEMICOLON, INT_KEYW, RET_KEYW, INT_LITERAL, IDENTIFIER} TOKEN_TYPE;

//Tokenlist node, contains token data and pointer to next token (if available)
typedef struct token_t {
  char *value;
  TOKEN_TYPE type;
  struct token_t *next;
} token_t;

//Tokenlist type
typedef struct tokenlist_t {
  token_t *head;
  token_t *tail;
} tokenlist_t;

char sourcePath[LEN_PATH] = "";
int tokenListSize;

//Token Regex Types
//Single char keywords
regex_t openBrace;
regex_t closeBrace;
regex_t openParen;
regex_t closeParen;
regex_t semicolon;

//Multi char keywords
regex_t int_keyw;
regex_t ret_keyw;
regex_t identifier;
regex_t int_literal;

regex_t keywords[9];

/**
 * initRegexp()
 * Initializes regular expression variables for use in lexing, and adds them to the keywords list.
 *
 * return void
 **/
void initRegexp(){
  int flag = 0;
  flag += regcomp(&openBrace, "{", 0);
  flag += regcomp(&closeBrace, "}", 0);
  flag += regcomp(&openParen, "(", 0);
  flag += regcomp(&closeParen, ")", 0);
  flag += regcomp(&semicolon, ";", 0);
  flag += regcomp(&int_keyw, "int", 0);
  flag += regcomp(&ret_keyw, "return", 0);
  flag += regcomp(&int_literal, "[0-9]\\+", 0);
  flag += regcomp(&identifier, "[a-zA-Z]\\w*", 0);
  if(flag > 0){
    fprintf(stderr, "Failed to init 1 or more regular expressions.\n");
    exit(1);
  }
  //Assign keywords to regex_t types
  keywords[0] = openBrace;
  keywords[1] = closeBrace;
  keywords[2] = openParen;
  keywords[3] = closeParen;
  keywords[4] = semicolon;
  keywords[5] = int_keyw;
  keywords[6] = ret_keyw;
  keywords[7] = int_literal;
  keywords[8] = identifier;
}

/**
 * *createToken(char *value, TOKEN_TYPE type)
 * Creates a new token data type, and returns its pointer.
 *
 * param value - String containing the value of the token ('main'/'int'/'16', etc)
 * param type - Enum of the type of token (for '16, type would be INT_LITERAL, value of 5)
 * return token_t* - returns a pointer to the newly created token
 **/
token_t *createToken(char *value, TOKEN_TYPE type){
  if(value == NULL){
    fprintf(stderr, "Attempted to create token with null value\n");
    exit(1);
  }
  token_t *newToken = malloc(sizeof(token_t)*1);
  if(newToken == NULL){
    fprintf(stderr, "Failed to allocate space for new token.\n");
    exit(1);
  }
  newToken->value = value;
  newToken->type = type;
  newToken->next = NULL;
  return newToken;
}

/**
 * freeRegs()
 * Free's all of the created keyword regular expressions.
 *
 * return void
 **/
void freeRegs(){
  int i;
  for(i = 0; i < NUM_KEYWORDS; i++){
    regfree(&keywords[i]);
  }
}

/**
 * printSubstr(char *line, int start, int end)
 * Prints a substring, given start/end indexes and the string.
 *
 * param line - the string to print the substring from.
 * param start - the starting index to print the substring from.
 * param end - the ending index of the substring being printed.
 **/
void printSubstr(char *line, int start, int end){
  int i;
  for(i = 0; i < end-start; i++){
    putchar(line[start+i]);
  }
}

/**
 * initTokenlist()
 * Allocates space and initializes an empty token list.
 *
 * return tokenlist_t* - returns the newly initialized token list
 **/
tokenlist_t *initTokenlist(){
  tokenlist_t *tokens;
  tokens = (tokenlist_t*) malloc(sizeof(tokenlist_t));
  if(tokens == NULL){
    fprintf(stderr, "Failed to allocate space for tokenlist.\n");
  }
  tokens->head = NULL;
  tokens->tail = NULL;
  return tokens;
}

/**
 * popToken(tokenlist_t *tokens)
 * Pops a token off of the linked list, and returns it
 *
 * param *tokens - the tokenlist to pop a token from
 * return token_t* - returns a pointer to the popped token
 **/
token_t *popToken(tokenlist_t *tokens){
  if(tokens->head == NULL)
    return NULL;
  //If one token left in list...
  token_t *popped = tokens->head;
  if(tokens->head == tokens->tail){
    tokens->head = NULL;
    tokens->tail = NULL;
      return popped;
  }
  tokens->head = popped->next;
  popped->next = NULL;
  return popped;
}

/**
 * appendToken(tokenlist_t *tokens, token_t *token)
 * Appends the provided token to the provided tokenlist
 *
 * param *tokens - the token list to append the token to
 * param *token - the token to append to the token list
 * return void
 **/
void appendToken(tokenlist_t *tokens, token_t *token){

  if(tokens == NULL){
    fprintf(stderr, "Attempted to append token to null tokenlist\n");
    return;
  }
  if(token == NULL){
    fprintf(stderr, "Attempted to append null token to tokenlist\n");
    return;
  }
  if(tokens->head == NULL){
    tokens->head = token;
    tokens->tail = token;
    token->next = NULL;
    return;
  }
  else if(tokens->tail != NULL){
    tokens->tail->next = token;
    tokens->tail = token;
    return;
  }
}

/**
 * *lex()
 * Lex's the source file (path provided in argv[1] of main), and returns a list of valid tokens
 *
 * return tokenlist_t* - returns a list of valid tokens from the source file
 **/
tokenlist_t *lex(){
  FILE *sourceFile;
  char *fileBuf = NULL;
  int numTokens = 0;
  tokenlist_t *tokens;

  //Attempt to open source code file
  sourceFile = fopen(sourcePath, "r");
  if(sourceFile == NULL){
    fprintf(stderr, "Failed to open source file %s\n", sourcePath);
    exit(1);
  }
  //Allocate space for buffer containing source file contents
  else{
    fseek(sourceFile, 0, SEEK_END);
    int sourceLen = ftell(sourceFile);
    fseek(sourceFile, 0, SEEK_SET);
    fileBuf = malloc(sizeof(char) * sourceLen + 1);
    if(fileBuf == NULL){
      fprintf(stderr, "Failed to allocate file buffer in Lexer.\n");
      exit(1);
    }
    fread(fileBuf, 1, sourceLen, sourceFile);
    fclose(sourceFile);
  }
  tokens = initTokenlist();
  //Lex the source file using regex
  char *line = 0;
  int lineNum = 0;

  printf("── lexing %s ──\n\n", sourcePath);
  line = strtok(fileBuf, "\n");
  //Parse line for tokens
  while(line){
    printf("line %d: %s\n", lineNum, line);
    //while inside line, parse and identify as many tokens as possible
    //update token search offset in line as tokens are identified in order
    //int main() { - should find 'int' 'main' '(' ')' '{'
    //The lowest index match, not the FIRST match should be used
    int i;
    regmatch_t pmatch = {-1, -1};
    int lineLen = strnlen(line, 1000);
    int minStart = lineLen;
    int minEnd = lineLen;
    TOKEN_TYPE tokType = -1;
    while(line[0] != '\0'){
      for(i = 0; i < NUM_KEYWORDS; i++){
        if(regexec(&keywords[i], line, 1, &pmatch, 0) == 0){
          //If lower index match is found, update minStart & minEnd
          if(pmatch.rm_so < minStart){
            minStart = pmatch.rm_so;
            minEnd = pmatch.rm_eo;
            tokType = (TOKEN_TYPE) i;
          }
        }
        //Need to add else for invalid token types...
      }
      printf("\tToken found: ");
      printSubstr(line, minStart, minEnd);
      puts("");
      token_t *newToken = createToken(strndup(&line[minStart], minEnd-minStart), tokType);
      appendToken(tokens, newToken);
      numTokens++;
      //printf("\t\tAdded token: %s\n", newToken->value);
      //Update line position (and line length) to remove identified token
      line += minEnd;
      lineLen -= minEnd;
      //Reset start/end indexes
      minStart = lineLen;
      minEnd = lineLen;
    }
    lineNum++;
    line = strtok(NULL, "\n");
  }
  printf("Number of tokens identified: %d\n", numTokens);
  tokenListSize = numTokens;
  free(fileBuf);
  free(line);
  return tokens;
}

/**
 * printTokens(tokenlist_t *tokens)
 * Prints the values of all tokens in the provided token list
 *
 * param *tokens - the list of tokens to print from
 * return void
 **/
void printTokens(tokenlist_t *tokens){
  printf("Tokens:\n");
  if(tokens->head == NULL)
    return;
  token_t *currToken = tokens->head;
  while(currToken != NULL){
    printf("%s\n", currToken->value);
    currToken = currToken->next;
  }
}


/**
 * freeTokens(tokenlist_t *tokens)
 * Frees all tokens in the provided token list, as well as the token list itself
 *
 * param *tokens - the token list to free
 * return void
 **/
void freeTokens(tokenlist_t *tokens){
  if(tokens == NULL)
    return;
  token_t *currToken = tokens->head;
  token_t *prev = NULL;
  while(currToken != NULL){
    prev = currToken;
    currToken = currToken->next;
    free(prev->value);
    free(prev);
  }
  free(tokens);
}

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
  printf(" - parsing complete -\n\n", sourcePath);
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
}

int main(int argc, char *argv[]) {
  if(argc < 2){
    fprintf(stderr, "Usage: %s <source code file>\n\n  This compiler should generate an assembly file, assemblable and linkable with:\n\tgcc <generated .s file> -m32 -o <output file> for x86.\n", argv[0]);
    exit(1);
  }
  strncpy(sourcePath, argv[1], LEN_PATH);
  //If source file extension is not .c, raise error and exit.
  if(strncmp(".c", &sourcePath[strnlen(sourcePath, LEN_PATH)-2], 2) != 0){
    fprintf(stderr, "Can only compile .c files!\n");
    exit(1);
  }
  initRegexp();
  tokenlist_t *tokens = lex();
  printf("Token List Size: %d\n", tokenListSize);
  //printTokens(tokens);
  puts("");
  astnode_t * progAST = parseProgram(tokens);
  printf("── printing AST ──\n");
  printAST(progAST);
  //Free's
  freeTokens(tokens);
  freeRegs();
  return 0;
}
