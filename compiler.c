#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <ctype.h>

#define LEN_PATH 4097
#define NUM_KEYWORDS 8

char sourcePath[LEN_PATH] = "";
char **tokens= NULL;
int tokenListSize;

//Token Regex Types
regex_t openBrace;
regex_t closeBrace;
regex_t openParen;
regex_t closeParen;
regex_t semicolon;
regex_t int_keyw;
regex_t ret_keyw;
regex_t identifier;
regex_t int_literal;

regex_t keywords[9];

void printTokens(){
  int i;
  printf("Tokens:\n");
  for(i = 0; i < tokenListSize; i++){
    printf("%s\n", tokens[i]);
  }
}

void printKeywordType(int i){
  switch (i) {
  case 0:
    printf("open bracket");
    break;
  case 1:
    printf("closed bracket");
    break;
  case 2:
    printf("open parentheses");
    break;
  case 3:
    printf("closed parentheses");
    break;
  case 4:
    printf("semicolon");
    break;
  case 5:
    printf("int keyword");
    break;
  case 6:
    printf("return keyword");
    break;
  case 7:
    printf("int literal");
    break;
  default:
    printf("identifier");
  }
}

void initRegexp(){
  int flag = 0;
  flag += regcomp(&openBrace, "{", 0);
  flag += regcomp(&closeBrace, "}", 0);
  flag += regcomp(&openParen, "(", 0);
  flag += regcomp(&closeParen, ")", 0);
  flag += regcomp(&semicolon, ";", 0);
  flag += regcomp(&int_keyw, "int", 0);
  flag += regcomp(&ret_keyw, "return", 0);
  flag += regcomp(&identifier, "[a-zA-Z]\\w*", 0);
  flag += regcomp(&int_literal, "[0-9]\\+", 0);
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
  //keywords[8] = identifier;
}

void lex(){
  FILE *sourceFile;
  char *fileBuf = NULL;
  int numTokens = 0;

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
    fileBuf = malloc(sizeof(char) * sourceLen);
    if(fileBuf == NULL){
      fprintf(stderr, "Failed to allocate file buffer in Lexer.\n");
      exit(1);
    }
    fread(fileBuf, 1, sourceLen, sourceFile);
    fclose(sourceFile);
  }
  //Allocate starting space for token list, token strings allocated as it goes
  tokens = malloc(sizeof(char *) * 500);
  //Lex the source file using regex
  char *line = 0;
  int lineNum = 0;

  printf("-- lexing %s --\n\n", sourcePath);
  line = strtok(strdup(fileBuf), "\n");
  //Parse line for tokens
  char currToken[1000] = "";
  while(line){
    printf("line %d: %s\n", lineNum, line);
    //Check, building current token char by char
    strncpy(currToken, line, 1);
    while(*line != '\0'){
      int i;
      //Build current token until single keyword is found
      for(i = 0; i < NUM_KEYWORDS; i++){
        //Ignore whitespace, it can't part of a keyword
        if(isspace(*currToken)){
          strncpy(currToken, "", 1);
          break;
        }
        //If next character in line is a keyword, return token
        if(regexec(&keywords[i], currToken, 0, NULL, 0) == 0){
          printf("\tToken found (");
          printKeywordType(i);
          printf("): %s\n", currToken);
          tokens[numTokens] = malloc(sizeof(char)*100);
          if(tokens[numTokens] == NULL){
            fprintf(stderr, "Failed to allocate memory for token \"%s\"\".\n", currToken);
            exit(1);
          }
          strncpy(tokens[numTokens], currToken, 99);
          strncpy(currToken, "", 1);
          numTokens++;
          break;
        }
        //If next is a single char keyword, the this current token is an identifier
        else if(regexec(&keywords[i], strndup(&line[1], 1), 0, NULL, 0) == 0){
          if(regexec(&identifier, currToken, 0, NULL, 0) == 0){
            printf("\tToken found (");
            printKeywordType(9);
            printf("): %s\n", currToken);
            tokens[numTokens] = malloc(sizeof(char)*100);
            if(tokens[numTokens] == NULL){
                fprintf(stderr, "Failed to allocate memory for token \"%s\"\".\n", currToken);
                exit(1);
            }
          strncpy(tokens[numTokens], currToken, 99);
            strncpy(currToken, "", 1);
            numTokens++;
            break;
          }
        }
      }
      line++;
      strncat(currToken, line, 1);
    }
    //Increment line counter
    lineNum++;
    line = strtok(NULL, "\n");
  }
  printf("Number of tokens identified: %d\n", numTokens);
  tokens = realloc(tokens, sizeof(char*) * numTokens + 1);
  if(tokens == NULL){
    fprintf(stderr, "Failed to reallocate tokens to size %d\n", numTokens);
    exit(1);
  }
  tokenListSize = numTokens;
}

void freeTokens(){
  int i;
  for(i = 0; i < tokenListSize; i++){
    free(tokens[i]);
  }
  free(tokens);
}

int main(int argc, char *argv[]) {
  if(argc < 2){
    fprintf(stderr, "Usage: %s <source file>\n\nThis compiler should generate an object file, linkable using gcc for x86.\n", argv[0]);
    exit(1);
  }
  strncpy(sourcePath, argv[1], LEN_PATH);
  initRegexp();
  if(regexec(&int_literal, "49", 0, NULL, 0) != 0){
    fprintf(stderr, "Houston, we have a problem...\n");
    exit(1);
  }
  lex();
  printf("Token List Size: %d\n", tokenListSize);
  //printTokens();
  free(tokens);
  return 0;
}
