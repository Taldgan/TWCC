#ifndef GEN_H_
#define GEN_H_

#include "parse.h"
#include <stdio.h>

FILE *getOutFile();
void generate(astnode_t *root);

#endif // GEN_H_
