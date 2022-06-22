#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <ctype.h>
#include "lex.h"

//Token Regex Types
//Single char keywords
regex_t openBrace, closeBrace, openParen, closeParen, semicolon;

//Multi char keywords
regex_t int_keyw, ret_keyw, identifier, int_literal;

regex_t negation, bitwise_comp, logic_neg;

regex_t add_op, mult_op, div_op;

regex_t and_op, or_op, eq_to, neq_to, lt_op, le_op, gt_op, ge_op;

regex_t mod_op, bit_and, bit_or, bit_xor, shift_left, shift_right;

regex_t keywords[NUM_KEYWORDS];

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
  flag += regcomp(&negation, "-", 0);
  flag += regcomp(&bitwise_comp, "~", 0);
  flag += regcomp(&logic_neg, "!", 0);
  flag += regcomp(&add_op, "+", 0);
  flag += regcomp(&mult_op, "*", 0);
  flag += regcomp(&div_op, "/", 0);
  flag += regcomp(&and_op, "&&", 0);
  flag += regcomp(&or_op, "||", 0);
  flag += regcomp(&eq_to, "==", 0);
  flag += regcomp(&neq_to, "!=", 0);
  flag += regcomp(&lt_op, "<", 0);
  flag += regcomp(&le_op, "<=", 0);
  flag += regcomp(&gt_op, ">", 0);
  flag += regcomp(&ge_op, ">=", 0);
  flag += regcomp(&mod_op, "%", 0);
  flag += regcomp(&bit_and, "&", 0);
  flag += regcomp(&bit_or, "|", 0);
  flag += regcomp(&bit_xor, "\\^", 0);
  flag += regcomp(&shift_left, "<<", 0);
  flag += regcomp(&shift_right, ">>", 0);
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
  keywords[9] = negation;
  keywords[10] = bitwise_comp;
  keywords[11] = logic_neg;
  keywords[12] = add_op;
  keywords[13] = mult_op;
  keywords[14] = div_op;
  keywords[15] = and_op;
  keywords[16] = or_op;
  keywords[17] = eq_to;
  keywords[18] = neq_to;
  keywords[19] = lt_op;
  keywords[20] = le_op;
  keywords[21] = gt_op;
  keywords[22] = ge_op;
  keywords[23] = mod_op;
  keywords[24] = bit_and;
  keywords[25] = bit_or;
  keywords[26] = bit_xor;
  keywords[27] = shift_left;
  keywords[28] = shift_right;
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
  tokens->numTokens = 0;
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
 * prependToken(tokenlist_t *tokens, token_t *token)
 * Prepends the provided token to the provided tokenlist
 *
 * param *tokens - the token list to prepend the token to
 * param *token - the token to prepend to the token list
 * return void
 **/
void prependToken(tokenlist_t *tokens, token_t *token){
  if(token == NULL){
    return;
  }
  if(tokens->head == NULL){
    tokens->head = token;
    tokens->tail = token;
    return;
  }
  token->next = tokens->head;
  tokens->head = token;
}

/**
 * peek(tokenlist_t *tokens)
 * Peeks the token list, returning its top element
 *
 * param *tokens - the token list to peek
 * return token_t* - returns pointer to head of the token list
 **/
token_t *peek(tokenlist_t *tokens){
  if(tokens == NULL){
    return NULL;
  }
  return tokens->head;
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

  //printf("── lexing %s ──\n\n", sourcePath);
  line = strtok(fileBuf, "\n");
  //Parse line for tokens
  while(line){
    //printf("line %d: %s\n", lineNum, line);
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
      //Chew through whitespace in line
      if(isspace(line[0]) != 0){
        line++;
        continue;
      }
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
      //printf("\tToken found: ");
      //printSubstr(line, minStart, minEnd);
      //puts("");
      token_t *newToken = createToken(strndup(&line[minStart], minEnd-minStart), tokType);
      appendToken(tokens, newToken);
      tokens->numTokens++;
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
  //printf("Number of tokens identified: %d\n", tokens->numTokens);
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
