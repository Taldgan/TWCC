#include <stdio.h>
#include <strings.h>

int main(int argc, char *argv[]) {
  if(argc < 2){
    fprintf(stderr, "Usage: %s <source file>\n\nThis compiler should generate an object file, linkable using gcc for x86.\n", argv[0]);
  }
  return 0;
}
