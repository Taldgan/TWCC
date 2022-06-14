#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <ctype.h>

#define LEN_PATH 4097
#define NUM_KEYWORDS 9

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
  //Assign 'single' keywords
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

void freeRegs(){
  int i;
  for(i = 0; i < NUM_KEYWORDS; i++){
    regfree(&keywords[i]);
  }
}

void printSubstr(char *line, int start, int end){
  int i;
  for(i = 0; i < end-start; i++){
    putchar(line[start+i]);
  }
}

//addToken for global array tokenList, deprecated
//char* addToken(char *line, int start, int end, int* numTokens){
//  char *token = NULL;
//  token = malloc(sizeof(char)*100);
//  if(token == NULL){
//    fprintf(stderr, "Failed to allocate space for token.\n");
//    exit(1);
//  }
//  strncat(token, &line[start], end-start);
//  tokens[*numTokens] = token;
//  (*numTokens)++;
//  return token;
//}

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

token_t *popToken(tokenlist_t *tokens){
  if(tokens->head == NULL)
    return NULL;
  token_t *popped = tokens->head;
  tokens->head = popped->next;
  popped->next = NULL;
  return popped;
}

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

  printf("-- lexing %s --\n\n", sourcePath);
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
      }
      printf("\tToken found: ");
      printSubstr(line, minStart, minEnd);
      puts("");
      token_t *newToken = createToken(strndup(&line[minStart], minEnd-minStart), tokType);
      appendToken(tokens, newToken);
      printf("\t\tAdded token: %s\n", newToken->value);
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

int main(int argc, char *argv[]) {
  if(argc < 2){
    fprintf(stderr, "Usage: %s <source code file>\n\nThis compiler should generate an object file, linkable with gcc for x86.\n", argv[0]);
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
  printTokens(tokens);

  //Free's
  freeTokens(tokens);
  freeRegs();
  return 0;
}
