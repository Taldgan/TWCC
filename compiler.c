#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <ctype.h>

#define LEN_PATH 4097
#define NUM_KEYWORDS 8

char sourcePath[LEN_PATH] = "";

//Token regex types
//{
//}
//(
//)
//;
//int
//return
//identifier (any word with any amount of whitespace + any character after)
//integer literal (1 or more digits)
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
  flag += regcomp(&int_literal, "[0-9]+", 0);
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

char **lex(){
  FILE *sourceFile;
  char *fileBuf = NULL;
  //char **tokens = {};
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
  //Lex the source file using regex
  char *line = 0;
  int lineNum = 0;

  printf("-- compiling %s --\n%s\n\n", sourcePath, fileBuf);
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
        //If next character in line is single keyword, return token
        if(isspace(*currToken)){
          strncpy(currToken, "", 1);
          break;
        }
        if(regexec(&keywords[i], currToken, 0, NULL, 0) == 0){
          printf("\tToken found: %s\n", currToken);
          strncpy(currToken, "", 1);
          numTokens++;
          break;
        }
        else if(regexec(&keywords[i], strndup(&line[1], 1), 0, NULL, 0) == 0){
          printf("\tToken found: %s\n", currToken);
          strncpy(currToken, "", 1);
          numTokens++;
          break;
        }
      }
      line++;
      strncat(currToken, line, 1);
    }
    //Increment line counter
    lineNum++;
    line = strtok(NULL, "\n");
  }
  return 0;
}

int main(int argc, char *argv[]) {
  if(argc < 2){
    fprintf(stderr, "Usage: %s <source file>\n\nThis compiler should generate an object file, linkable using gcc for x86.\n", argv[0]);
    exit(1);
  }
  strncpy(sourcePath, argv[1], LEN_PATH);
  initRegexp();
  lex();
  return 0;
}
