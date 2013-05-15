#ifndef BN_ADD
#define BN_ADD

#include "bn_math_int.h"

void bn_add(bn*a, bn*b, bn*out){
  int64 tmp=0;
  int maxlen;
  int i;
  if(a->len >= b->len){
    maxlen= a->len;
    for(i=b->len; i<a->len; ++i)
      b->n[i]=0;
  }else{
    maxlen= b->len;
    for(i=a->len; i<b->len; ++i)
      a->n[i]=0;
  }
  for(i=0; i<maxlen; ++i){
    tmp+= (int64)a->n[i] + b->n[i];
    out->n[i]= tmp&0xffffffff;
    tmp= tmp>>32;
  }
#ifdef pldbg
  if(maxlen >= BNSIZE) dbgmsg("bn_add bn_add overflow error\n");
  if(tmp!=0 && tmp!=1) dbgmsg("bn_add bn_add x= x!=0 cannot-click-to-[0,1] error.");
#endif
  out->n[maxlen]=tmp;
  out->len= maxlen + tmp;
}
int test_case_bn_add_(){
  int ret=0;
  bn a;
  bn b;
  bn c;
  bn_sinput("12345678",&a);
  bn_sinput("87654321",&b);
  bn_sinput("99999999",&c);
  bn_add(&a,&b,&a);
  if(bn_cmp(&a,&c)) 
    ++ret, bn_print_prefix("bn_add bn_add 12345678+87654321==99999999 !",&a,"\n");

  bn_sinput("82345678",&a);
  bn_sinput("1111111187654321",&b);
  bn_sinput("1111111209999999",&c);
  bn_add(&a,&b,&a);
  if(bn_cmp(&a,&c)) 
    ++ret, bn_print_prefix("bn_add bn_add 82345678+1111111187654321==1111111209999999 !",&a,"\n");

  bn_sinput("1111111182345678",&a);
  bn_sinput("87654321",&b);
  bn_sinput("1111111209999999",&c);
  bn_add(&a,&b,&a);
  if(bn_cmp(&a,&c)) 
    ++ret, bn_print_prefix("bn_add bn_add 1111111182345678+87654321==1111111209999999 !",&a,"\n");
  
  bn_sinput("0101010101010101",&a);
  bn_sinput("1010101010101010",&b);
  bn_sinput("1111111111111111",&c);
  bn_add(&a,&b,&a);  
  if(bn_cmp(&a,&c)) 
    ++ret, bn_print_prefix("bn_add bn_add 1010101010101010+0101010101010101==1111111111111111 !",&a,"\n");
  
  bn_sinput("0fffffffffffffff",&a);
  bn_sinput("1ffffffffffffffe",&c);
  bn_add(&a,&a,&a);  
  if(bn_cmp(&a,&c)) 
    ++ret, bn_print_prefix("bn_add bn_add 0fffffffffffffff+0fffffffffffffff==1ffffffffffffffe !",&a,"\n");
  
  return ret;
}
int bn_sub(bn*a,bn*b,bn*out){
  int64 tmp=0;
  int i;
  for(i=0; i<b->len; ++i){
    tmp= ((int64)0x200000000 + (int64)a->n[i]) - b->n[i] - tmp;
    out->n[i]= tmp&0xffffffff;
    tmp= 2-(tmp>>32);
  }
  while(tmp!=0){
#ifdef pldbg
    if(i >= BNSIZE) dbgmsg("bn_add bn_sub overflow error\n");
#endif
    tmp= ((int64)0x200000000 + (int64)a->n[i]) - tmp;
    out->n[i]= tmp&0xffffffff;
    tmp= 2-(tmp>>32);
    i++;
  }
  while(i<a->len){
    out->n[i]=a->n[i];
    ++i;
  }
  out->len= a->len;
  bn_trim(out);
  return tmp;
}
int test_case_bn_sub(){
  int ret=0;
  bn a;
  bn b;
  bn c;
  bn_sinput("99999999",&a);
  bn_sinput("12345678",&b);
  bn_sinput("87654321",&c);
  bn_sub(&a,&b,&a);
  if(bn_cmp(&a,&c)) 
    ++ret, bn_print_prefix("bn_add bn_sub 99999999-12345678==87654321 !",&a,"\n");
  
  bn_sinput("1111111111111111",&a);
  bn_sinput("0101010101010101",&b);
  bn_sinput("1010101010101010",&c);
  bn_sub(&a,&b,&a);  
  if(bn_cmp(&a,&c)) 
    ++ret, bn_print_prefix("bn_add bn_sub 1111111111111111-1010101010101010==0101010101010101 !",&a,"\n");
  
  bn_sinput("1ffffffffffffffe",&a);
  bn_sinput("0fffffffffffffff",&b);
  bn_sinput("0fffffffffffffff",&c);
  bn_sub(&a,&b,&a);  
  if(bn_cmp(&a,&c)) 
    ++ret, bn_print_prefix("bn_add bn_sub 1ffffffffffffffe-0fffffffffffffff==0fffffffffffffff !",&a,"\n");
  
  return ret;
}


int test_case_bn_add(){
  int ret=0;
  ret+= test_case_bn_add_();
  ret+= test_case_bn_sub();
  return ret;
}

#endif
