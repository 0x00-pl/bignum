#include <stdlib.h>
#include <time.h>

#include "bn_type.h"
#include "int_math.h"
#include "bn_math_int.h"
#include "bn_add.h"
#include "bn_mul.h"
#include "bn_pow.h"
#include "bn_prime.h"


//#ifdef pldbg
int main(){
  //srand(time(NULL));
  int ret=0;
  ret+= test_case_int_math();
  ret+= test_case_bn_type();
  ret+= test_case_bn_math_int();
  ret+= test_case_bn_add();
  ret+= test_case_bn_mul();
  ret+= test_case_bn_pow();
  ret+= test_case_bn_prime();
  printf("test_case ret: %d\n",ret);
  bn p;
  int n=10;
  while(n-->0){
    bn_make_prime_1024(&p);
    bn_print_prefix("",&p," is prime.\n");
  }
  return 0;
}
//#endif
