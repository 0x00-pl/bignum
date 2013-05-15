#ifndef BN_PRIME
#define BN_PRIME

#include "primes.h"
#include "bn_pow.h"

int bn_test_prime_small(bn*a){
  int i;
  for(i=0; primes[i]!=0; ++i){
    if(bn_mod_int(a,primes[i])==0) return 0;
  }
  return 1;
}

int bn_test_prime_base(bn*a){
  bn skey,sone,sa1,res;
  int i;
  bn_set(&sone,1);
  bn_copy(a,&sa1);
  bn_sub_int(&sa1,1);
  int n=5;
  while(n-->0){
    //make key
    skey.len=a->len;
    for(i=0;i<skey.len;++i){
      skey.n[i]=rand();
    }
    while(skey.n[skey.len-1] % a->n[a->len-1]!=0) 
      skey.n[skey.len-1]>>=1;
   
    bn_pow_mod(&skey,&sa1,a,&res);
    if(!(res.len==1 && res.n[0]==1))
      return 0;
  }
  return 1;
}

int bn_test_prime_miller_rabin_key(bn*k,bn*m,int32 pblock, int pz){
  bn p,kp2n,kp2n2,rmd,rmd2;
  bn_copy(m,&p); p.n[0]&=~1;//p=m-1
#ifdef pldbg
  if(m->n[0]&1!=1) dbgmsg("bn_prime bn_test_prime_miller_rabin_key tested-prime-is-div-by-2 error\n");
#endif
  bn_copy(k,&kp2n);
  bn_set(&rmd,1);
  int b=0;
  bn* pkp2n=&kp2n;
  bn* pkp2n2=&kp2n2;
  bn* prmd=&rmd;
  bn* prmd2=&rmd2;
  int intbit= 32-pz;
  while(b < p.len){
    while(intbit-->0){
      if((pblock&1)!=0){
	//rmd= (rmd*kp2n)%m
	bn_mul_mod(prmd,pkp2n,m,prmd2);
	bn*t=prmd; prmd=prmd2; prmd2=t;
      }
      //kp2n= (kp2n*kp2n)%m
      bn_sqr_mod(pkp2n,m,pkp2n2);
      bn*t=pkp2n; pkp2n=pkp2n2; pkp2n2=t;
      pblock>>=1;
    }
    ++b;
    pblock=p.n[b];
    intbit=32;
  }
  //if prmd==1 return true;
  if(prmd->len==1 && prmd->n[0]==1){
    return 1;
  }
  bn m1;
  bn_copy(m,&m1);
  bn_sub_int(&m1,1);
  while(pz-->1){
    if(bn_cmp(&m1,prmd)==0) return 1;
    bn_sqr_mod(prmd,m,prmd2);
    bn*t=prmd; prmd=prmd2; prmd2=t;
  }
  return bn_cmp(&m1,prmd)==0;
}


int bn_test_prime_miller_rabin(bn*a){
  int i;
  int az=1;
  int32 ablock=a->n[0];
  ablock>>=1;
  while((ablock&1)==0){
    ablock>>=1;
    az++;
  }
  int n=20;
  while(n-->0){
    //randk= rand_range(a)
    bn randk;
    randk.len=a->len;
    for(i=0;i<randk.len-1;++i){
      randk.n[i]=rand();
    }
    randk.n[a->len-1]= a->n[a->len-1]-1;
    if(randk.n[a->len-1]==0){
      --randk.len;
    }
    // test
    int flag= bn_test_prime_miller_rabin_key(&randk,a,ablock,az);
    if(flag==0) return 0;
  }
  return 1;
}

int bn_test_prime(bn*a){
  if(bn_test_prime_small(a)==0) return 0;
  int flag;
  //flag= bn_test_prime_base(a);
  flag= bn_test_prime_miller_rabin(a);
#ifdef pldbg
  int flag1,flag2;
  flag2= bn_test_prime_miller_rabin(a);
  flag1= bn_test_prime_base(a);
  if(flag1!=flag2) dbgmsg("bn_prime bn_test_prime error\n");
#endif
  return flag;
}
void bn_make_prime_1024(bn*out){
  out->len=1024/32;
  int i=1024/32;
  while(i-->0)
    out->n[i]=rand();
  out->n[0]|=1;
  int dlt=2;
  while(1){
    if(bn_test_prime(out))
      return;
    bn_add_int(out,dlt);
    dlt+=2;
  }
}

int test_case_bn_test_prime(){
  int ret=0;
  bn a;
  bn_sinput("35EBF13FCB0C0A77AFDB04F028317F47",&a);
  if(bn_test_prime(&a)!=1) 
    ++ret, printf("bn_prime bn_test_prime 35EBF13FCB0C0A77AFDB04F028317F47 fault\n");
  
  bn_sinput("2FEA51D3F9987D95C487887D32DE6214",&a);
  if(bn_test_prime(&a)!=0) 
    ++ret, printf("bn_prime bn_test_prime 2FEA51D3F9987D95C487887D32DE6214 fault\n");

  bn_sinput("A8C5781F70C4D457BEC02740D8B9B363",&a);
  if(bn_test_prime(&a)!=1) 
    ++ret, printf("bn_prime bn_test_prime A8C5781F70C4D457BEC02740D8B9B363 fault\n");

  bn_sinput("A982529927620C6FE83D8BF52D046C27",&a);
  if(bn_test_prime(&a)!=1) 
    ++ret, printf("bn_prime bn_test_prime A982529927620C6FE83D8BF52D046C27 fault\n");

  bn_sinput("26BC3A81E159CCFFDC411E71111EE7A7",&a);
  if(bn_test_prime(&a)!=1) 
    ++ret, printf("bn_prime bn_test_prime 26BC3A81E159CCFFDC411E71111EE7A7 fault\n");
  return ret;
}

int test_case_bn_prime(){
  int ret=0;
  int n=1;
  while(n-->0)
    ret+= test_case_bn_test_prime();
  return ret;
}


#endif
