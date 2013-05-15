#ifndef BN_MATH_INT
#define BN_MATH_INT

#include "bn_type.h"
#include "int_math.h"

void bn_add_int(bn* a, int32 n){
  int i=0;
  while(n!=0){
#ifdef pldbg
    if(BNSIZE<i) dbgmsg("bn_math_int bn_add_int overflow error.");
#endif
    if(i>=a->len) a->n[i]=0;
    int64 tmp= (int64)a->n[i]+n;
    a->n[i]= tmp&0xffffffff;
    n= tmp>>32;
    ++i;
  }
  if(i > a->len) a->len= i;
}
void bn_add_int64(bn* a, int64 n){
  int i=0;
  while(n!=0){
#ifdef pldbg
    if(BNSIZE<i) dbgmsg("bn_math_int bn_add_int overflow error.");
#endif
    if(i>=a->len) a->n[i]=0;
    int64 tmp= (int64)a->n[i]+(n&0xffffffff);
    a->n[i]= tmp&0xffffffff;
    n= (tmp>>32) + (n>>32);
    ++i;
  }
  if(i > a->len) a->len= i;
}
int test_case_bn_add_int(){
  int ret=0;
  bn a;
  bn b;
  bn_sinput("00000001",&a);
  bn_add_int(&a,1);
  bn_sinput("00000002",&b);
  if(bn_cmp(&a,&b)) 
    ++ret, bn_print_prefix("bn_math_int bn_add_int 1+1==2 !",&a,"\n");
  
  bn_sinput("ffffffff",&a);
  bn_add_int(&a,1);
  bn_sinput("0000000100000000",&b);
  if(bn_cmp(&a,&b)) 
    ++ret, bn_print_prefix("bn_math_int bn_add_int ffffffff+1==100000000 !",&a,"\n");
  
  bn_sinput("ffffffff",&a);
  bn_add_int(&a,0xffffffff);
  bn_sinput("00000001fffffffe",&b);
  if(bn_cmp(&a,&b)) 
    ++ret, bn_print_prefix("bn_math_int bn_add_int ffffffff+ffffffff==00000001fffffffe  !",&a,"\n");
  
  
  bn_sinput("ffffffffffffffff",&a);
  bn_add_int64(&a,0xffffffffffffffff);
  bn_sinput("00000001fffffffffffffffe",&b);
  if(bn_cmp(&a,&b)) 
    ++ret, bn_print_prefix("bn_math_int bn_add_int64 00000001fffffffffffffffe !",&a,"\n");

  return ret;
}

void bn_sub_int(bn* a, int32 n){
    int i=0;
    while(n!=0){
#ifdef pldbg
    if(BNSIZE<i) dbgmsg("bn_math_int bn_sub_int overflow error.");
#endif
    if(i>=a->len) a->n[i]=0;
    int64 tmp= ((int64)1<<32)+ a->n[i]- n;
    a->n[i]= tmp&0xffffffff;
    n= 1-(tmp>>32);
    ++i;
  }
  if(i > a->len) a->len= i;
  bn_trim(a);
}
int test_case_bn_sub_int(){
  int ret=0;
  bn a;
  bn b;
  bn_sinput("ffffffff",&a);
  bn_sub_int(&a,0xffffffff);
  bn_sinput("00000000",&b);
  bn_trim(&b);
  if(bn_cmp(&a,&b)) 
    ++ret, bn_print_prefix("bn_math_int bn_sub_int ffffffff-ffffffff==0 !",&a,"\n");
  
  bn_sinput("0000000100000000",&a);
  bn_sub_int(&a,1);
  bn_sinput("ffffffff",&b);
  if(bn_cmp(&a,&b)) 
    ++ret, bn_print_prefix("bn_math_int bn_sub_int 100000000-1==ffffffff !",&a,"\n");
  
  bn_sinput("100000000000000000000000",&a);
  bn_sub_int(&a,1);
  bn_sinput("0fffffffffffffffffffffff",&b);
  if(bn_cmp(&a,&b)) 
    ++ret, bn_print_prefix("bn_math_int bn_sub_int 100000000000000000000000-1==0fffffffffffffffffffffff  !",&a,"\n");
  
  return ret;
}

void bn_mul_int(bn* a, int32 n, bn* out){
    int i;
    int64 tmp=0;
    for(i=0; i<a->len; ++i){
      tmp= tmp + (int64)a->n[i] * n;
      out->n[i]= tmp&0xffffffff;
      tmp= tmp>>32;
    }
#ifdef pldbg
    if(BNSIZE<i) dbgmsg("bn_math_int bn_mul_int overflow error.");
    if(tmp!=0 && 1!=(tmp!=0)) dbgmsg("bn_math_int bn_mul_int x= x!=0 cannot-click-to-[0,1] error.");
#endif
    out->n[i]=tmp;
    out->len= a->len + !(!tmp);
}
void bn_add(bn*,bn*,bn*);
void bn_mul_int64(bn* a, int64 n, bn* out){
  //a*(nh<<32+nl)= a*nl + a*nh<<32
  bn t1;
  bn_mul_int(a,n&0xffffffff,out);
  bn_mul_int(a,n>>32,&t1);
  bn_shl(&t1,1);
  bn_add(out,&t1,out);
}
int test_case_bn_mul_int(){
  int ret=0;
  bn a;
  bn b;
  bn_sinput("0fffffff",&a);
  bn_mul_int(&a,0x10,&a);
  bn_sinput("fffffff0",&b);
  if(bn_cmp(&a,&b)) 
    ++ret, bn_print_prefix("bn_math_int bn_mul_int 0fffffff*0x10==fffffff0 !",&a,"\n");
  
  bn_sinput("0000000100000000",&a);
  bn_mul_int(&a,0xffffffff,&a);
  bn_sinput("ffffffff00000000",&b);
  if(bn_cmp(&a,&b)) 
    ++ret, bn_print_prefix("bn_math_int bn_mul_int 100000000*ffffffff==ffffffff00000000 !",&a,"\n");
  
  bn_sinput("11111111",&a);
  bn_mul_int(&a,0x11111111,&a);
  bn_sinput("0123456787654321",&b);
  if(bn_cmp(&a,&b)) 
    ++ret, bn_print_prefix("bn_math_int bn_mul_int 11111111*11111111==0123456787654321  !",&a,"\n");
  
  return ret;
}

int32 bn_mod_int(bn*a, int32 n){
  int64 tmp=0;
  int i;
  for(i=a->len; i>0; --i){ 
    tmp= tmp<<32;
    tmp= (tmp+a->n[i-1])%n;
  }
  return tmp;
}
int test_case_bn_mod_int(){
  int ret=0;
  bn a;
  bn_sinput("12345678",&a); 
  if(!(bn_mod_int(&a,0x10)==8))
    ++ret, printf("bn_math_int bn_mod_int 12345678 mod 0x10 ==8 !%d\n",bn_mod_int(&a,0x10));
 
  bn_sinput("0000000100000000",&a);
  if(!(bn_mod_int(&a,0xffffffff)==0x1))
    ++ret, printf("bn_math_int bn_mod_int 100000000 mod ffffffff=1 !%d\n",bn_mod_int(&a,0xffffffff));
  
  bn_sinput("111111112222222233333333",&a);
  if(!(bn_mod_int(&a,0x12345678)==0xba98793))
    ++ret, printf("bn_math_int bn_mod_int 111111112222222233333333 mod 12345678==ba98793 !%d\n",bn_mod_int(&a,0x12345678));
  
  return ret;
}

int32 bn_div_int(bn*a, int32 n, bn* out){
  int i;
  int64 tmp=0;
  int64 rmd=0;
  out->len= a->len;
  for(i=a->len; i>0; --i){
    rmd= rmd<<32;
    tmp= rmd+a->n[i-1];
    out->n[i-1]= tmp/n;
#ifdef pldbg
    if(tmp/n > 0xffffffff) dbgmsg("bn_math_int bn_div_int int32-overflow error");
#endif
    rmd= tmp%n;
  }
  bn_trim(out);
  return rmd;
}
int test_case_bn_div_int(){
  int ret=0;
  bn a;
  bn b;
  bn_sinput("12345678",&a);
  bn_div_int(&a,0x10,&a);
  bn_sinput("01234567",&b);
  if(bn_cmp(&a,&b)) 
    ++ret, bn_print_prefix("bn_math_int bn_div_int 12345678/10==01234567 !",&a,"\n");
  
  bn_sinput("0000000100000000",&a);
  bn_div_int(&a,0xffffffff,&a);
  bn_sinput("00000001",&b);
  if(bn_cmp(&a,&b)) 
    ++ret, bn_print_prefix("bn_math_int bn_div_int 100000000/ffffffff==1 !",&a,"\n");
  
  bn_sinput("0123456787654321",&a);
  bn_div_int(&a,0x11111111,&a);
  bn_sinput("11111111",&b);
  if(bn_cmp(&a,&b)) 
    ++ret, bn_print_prefix("bn_math_int bn_div_int 0123456787654321/11111111==11111111  !",&a,"\n");
  
  return ret;
}

int test_case_bn_math_int(){
  int ret=0;
  ret+=test_case_bn_add_int();
  ret+=test_case_bn_sub_int();
  ret+=test_case_bn_mul_int();
  ret+=test_case_bn_mod_int();
  ret+=test_case_bn_div_int();
  return ret;
}

#endif
