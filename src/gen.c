#include "parse.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


FILE *getOutFile(){
  int i;
  int pathLen = strnlen(sourcePath, LEN_PATH)-1;
  char outPath[LEN_PATH] = "";
  //find first occurrence of '/' (working backwards)
  for(i = pathLen; i > 0; i--){
    if(sourcePath[i] == '/'){
      i++;
      break;
    }
  }
  strncpy(outPath, &sourcePath[i], strnlen(&sourcePath[i], LEN_PATH)-1);
  strncat(outPath, "s", 2);
  printf("Output file: %s\n", outPath);
  FILE *outFile = fopen(outPath, "w");
  if(outFile == NULL){
    fprintf(stderr, "Failed to open output file %s for writing.\n", outPath);
    exit(1);
  }
  return outFile;
}

/**
 * generate(astnode_t *root, FILE *outFile)
 * Given a valid AST, generates assemblable assembly and writes it to a file.
 *
 * param *root - the PROGRAM node of the ast
 * param *outFile - the file pointer to write the assembly to
 * return void
 **/
void generate(astnode_t *root, FILE *outFile){
  if(root == NULL){
    fprintf(stderr, "Null AST node, cannot generate assembly.\n");
    exit(1);
  }
  astnode_t *currNode = root;
  //Now recursively traverse AST and use it to generate assembly
  if(currNode->nodeType == PROGRAM){
    generate(currNode->fields.children.left, outFile);
    return;
  }
  else if(currNode->nodeType == FUNCTION){
    char *funcName = currNode->fields.children.left->fields.strVal;
    fprintf(outFile, " .globl %s\n%s:\n", funcName, funcName);
    generate(currNode->fields.children.right, outFile);
    fprintf(outFile, " ret\n");
    return;
  }
  else if(currNode->nodeType == STATEMENT){
    //Not much to do with a statement yet, moving on...
    generate(currNode->fields.children.left, outFile);
    return;
  }
  else if(currNode->nodeType == INTEGER){
    fprintf(outFile, " movl $%d, %%eax\n", currNode->fields.intVal);
    return;
  }
  else if(currNode->nodeType == UN_OP){
    char opType = currNode->fields.children.left->fields.strVal[0];
    generate(currNode->fields.children.right, outFile);
    switch(opType){
      case '~':
        fprintf(outFile, " not %%eax\n");
        break;
      case '!':
        fprintf(outFile, " cmpl $0, %%eax\n");
        fprintf(outFile, " movl $0, %%eax\n");
        fprintf(outFile, " sete %%al\n");
        break;
      case '-':
        fprintf(outFile, " neg %%eax\n");
        break;
    }
    return;
  }
  fclose(outFile);
}
