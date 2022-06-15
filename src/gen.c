#include "parse.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void generate(astnode_t *root){
  if(root == NULL){
    fprintf(stderr, "Null AST node, cannot generate assembly.\n");
    exit(1);
  }
  //After checking for non-null root node, open output file
  char outPath[LEN_PATH] = "";
  strncpy(outPath, sourcePath, strnlen(sourcePath, LEN_PATH)-1);
  strncat(outPath, "s", 2);
  printf("Output file: %s\n", outPath);
  FILE *outFile = fopen(outPath, "w");
  if(outFile == NULL){
    fprintf(stderr, "Failed to open output file %s for writing.\n", outPath);
    exit(1);
  }
  //Now traverse AST and use it to generate assembly
}
