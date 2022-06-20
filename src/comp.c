//Compiler imports
#include "lex.h"
#include "parse.h"
#include "gen.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char sourcePath[LEN_PATH];


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
  //printf("Token List Size: %d\n", tokens->numTokens);
  printTokens(tokens);
  astnode_t * progAST = parseProgram(tokens);
  //printf("── printing AST ──\n");
  printAST(progAST);
  FILE *outFile = getOutFile();
  generate(progAST, outFile);
  //Free's
  freeTokens(tokens);
  freeRegs();
  return 0;
}
