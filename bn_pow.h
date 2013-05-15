#ifndef BN_POW
#define BN_POW

#include "bn_mul.h"

void bn_pow_mod_base_old(bn*a,bn*b,bn*m,bn*out){
  bn sapk,tsapk,tout;
  int i,j;
  int32 bi;
  bn_set(out,1);
  bn_copy(a, &sapk);
  for(i=0; i+1<b->len; ++i){
    bi= b->n[i];
    for(j=0; j<32; j+=2){
      if((bi&1) != 0){
      	bn_mul_mod(out,&sapk,m,&tout);
      	bn_copy(&tout,out);
      }
      //bn_mul_mod(&sapk,&sapk,m,&tsapk);
      bn_sqr_mod(&sapk,m,&tsapk);
      if((bi&2) != 0){
	bn_mul_mod(out,&tsapk,m,&tout);
      	bn_copy(&tout,out);
      }
      //bn_mul_mod(&tsapk,&tsapk,m,&sapk);
      bn_sqr_mod(&tsapk,m,&sapk);
      bi>>=2;
  }
  }
  for(bi= b->n[b->len-1]; bi!=0; bi>>=2){
    if((bi&1) != 0){
      bn_mul_mod(out,&sapk,m,&tout);
      bn_copy(&tout,out);
    }
    bn_sqr_mod(&sapk,m,&tsapk);
    if((bi&2) != 0){
      bn_mul_mod(out,&tsapk,m,&tout);
      bn_copy(&tout,out);
    }
    bn_sqr_mod(&tsapk,m,&sapk);
#ifdef pldbg
    bn dbgt,dbgt2;
    bn_mul_mod(&tsapk,&tsapk,m,&dbgt);
    bn_sqr_mod(&tsapk,m,&dbgt2);
    if(bn_cmp(&dbgt,&dbgt2)!=0){
      bn_print_prefix("tsapk ",&tsapk,"\n");
      bn_print_prefix("m     ",m,"\n");
      bn_print_prefix("sapk  ",&dbgt2,"\n");
      bn_print_prefix("dbgt  ",&dbgt,"\n\n");
    }
#endif
  }
}

void bn_pow_mod_base(bn*a,bn*e,bn*m,bn*out){
  bn ae,ae2,out2;
  bn_copy(a,&ae);
  bn_set(out,1);
  bn* t;
  bn* pae= &ae;
  bn* pae2= &ae2;
  bn* pout= out;
  bn* pout2= &out2;
  int b,i;
  int32 block;
  for(b=0;b<e->len;++b){
    block= e->n[b];
    for(i=0; i<32; ++i){
      if((block&1)==1){
	bn_mul_mod(pout,pae,m,pout2);
	t=pout; pout=pout2; pout2=t;
      }
      bn_sqr_mod(pae,m,pae2);
      t=pae; pae=pae2; pae2=t;
      block>>=1;
    }
  }
  //return out
  if(pout!=out){
    bn_copy(pout,out);
  }
}
void bn_pow_mod(bn*a,bn*e,bn*m,bn*out){
  bn_pow_mod_base(a,e,m,out);
}

int test_case_bn_pow_mod(){
  int ret=0;
  bn a,b,m,c,r;
  bn_sinput("000000123456789123456789",&a);
  bn_sinput("00000abc",&b);
  bn_sinput("0000001f2f3f4f5f",&m);
  bn_sinput("0000001e30ef2f83",&c);
  bn_pow_mod(&a,&b,&m,&r);
  if(bn_cmp(&r,&c)) 
    ++ret, bn_print_prefix("bn_pow bn_pow_mod 1e30ef2f83 !",&r,"\n");
  
  bn_sinput("C56518638BCE38FABF96564DE1BD3217",&a);
  bn_sinput("49C91D8F4457A4C4AC0D27EEA52F66B9",&b);
  bn_sinput("A56A3F3CA30CC33C6C9B21A358CF19C3",&m);
  bn_sinput("2BF4370CF5D6920C06E61E2F016DBD5E",&c);
  bn_pow_mod(&a,&b,&m,&r);
  if(bn_cmp(&r,&c)) 
    ++ret, bn_print_prefix("bn_pow bn_pow_mod 2BF4370CF5D6920C06E61E2F016DBD5E !",&r,"\n");
  
  bn_sinput("3BFD99C81D49EB6150F73940DC2FB583",&a);
  bn_sinput("1A77CA4B4475E5D89C42BF6086DF1CC5",&b);
  bn_sinput("AB78A8A36D5B09E62314EA6AE0090C4D",&m);
  bn_sinput("3CAF5CD5AE6808D16F23F49BA2CF54E8",&c);
  bn_pow_mod(&a,&b,&m,&r);
  if(bn_cmp(&r,&c)) 
    ++ret, bn_print_prefix("bn_pow bn_pow_mod 0x3CAF5CD5AE6808D16F23F49BA2CF54E8 !",&r,"\n");
  
  return ret;
}

int test_case_bn_pow(){
  int ret=0;
  ret+= test_case_bn_pow_mod();
  return ret;
}


#endif
