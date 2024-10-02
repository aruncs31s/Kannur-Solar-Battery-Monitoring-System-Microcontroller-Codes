#include <stdio.h>
#define k
int main(int argc, char *argv[]) {

#if defined(k) || defined(kk)
  printf("k and kk are defined\n");
#endif
}
