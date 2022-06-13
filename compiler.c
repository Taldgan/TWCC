#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <ctype.h>

#define LEN_PATH 4097
#define NUM_KEYWORDS 9

char sourcePath[LEN_PATH] = "";
char **tokens= NULL;
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

void printSubstr(char *line, int start, int end){
  int i;
  for(i = 0; i < end-start; i++){
    putchar(line[start+i]);
  }
}

char* addToken(char *line, int start, int end, int* numTokens){
  char *token = malloc(sizeof(char)*100);
  if(token == NULL){
    fprintf(stderr, "Failed to allocate space for token.\n");
    exit(1);
  }
  strncat(token, &line[start], end-start);
  tokens[*numTokens] = token;
  (*numTokens)++;
  return token;
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
    while(line[0] != '\0'){
      for(i = 0; i < NUM_KEYWORDS; i++){
        if(regexec(&keywords[i], line, 1, &pmatch, 0) == 0){
          //If lower index match is found, update minStart & minEnd
          if(pmatch.rm_so < minStart){
            minStart = pmatch.rm_so;
            minEnd = pmatch.rm_eo;
          }
        }
      }
      printf("\tToken found: ");
      printSubstr(line, minStart, minEnd);
      puts("");
      printf("\t\tAdded token: %s\n", addToken(line, minStart, minEnd, &numTokens));
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
  tokens = realloc(tokens, sizeof(char*) * numTokens + 1);
  if(tokens == NULL){
    fprintf(stderr, "Failed to reallocate tokens to size %d\n", numTokens);
    exit(1);
  }
  tokenListSize = numTokens;
  free(fileBuf);
}

void printTokens(){
  int i;
  printf("Tokens:\n");
  for(i = 0; i < tokenListSize; i++){
    printf("%s\n", tokens[i]);
  }
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
    fprintf(stderr, "Usage: %s <source code file>\n\nThis compiler should generate an object file, linkable with gcc for x86.\n", argv[0]);
    exit(1);
  }
  strncpy(sourcePath, argv[1], LEN_PATH);
  initRegexp();
  lex();
  printf("Token List Size: %d\n", tokenListSize);
  //printTokens();
  freeTokens();
  return 0;
}
