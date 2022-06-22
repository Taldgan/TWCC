#ifndef GEN_H_
#define GEN_H_

#include "parse.h"
#include <stdio.h>

FILE *getOutFile();
char *generateLabel();
void generate(astnode_t *root, FILE *outFile);

#endif // GEN_H_
