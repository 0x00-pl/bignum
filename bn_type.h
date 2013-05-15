#ifndef BN_TYPE
#define BN_TYPE

#include "dbg.h"

#define int32 unsigned int
#define int64 unsigned long long int
#define BNSIZE 64

typedef struct bn_inner{
  int len;
  int32 n[BNSIZE];
}bn;

typedef void(*bn_viewer)(bn*);

void bn_set(bn* a, int32 n){
#ifdef pldbg
  if(BNSIZE<1) dbgmsg("bn_type bn_set BNSIZE<1 error");
#endif
  a->n[0]=n;
  a->len=1;
}

void bn_set64(bn* a, int64 n){
#ifdef pldbg
  if(BNSIZE<2) dbgmsg("bn_type bn_set64 BNSIZE<2 error");
#endif
  a->n[0]= n&0xffffffff;
  a->n[1]= (n>>32)&0xffffffff;
  a->len= a->n[1]==0? 1: 2;
}

void bn_copy(bn*a,bn*out){
  int i;
  for(i=0; i<a->len; ++i){
    out->n[i]= a->n[i];
  }
  out->len= a->len;
}

int bn_cmp(bn*a,bn*b){
  if(a->len > b->len) return 1;
  if(a->len < b->len) return -1;
  int i=a->len;
  while(i>0){
    --i;
    if(a->n[i] > b->n[i]) return 1;
    if(a->n[i] < b->n[i]) return -1;
  }
  return 0;
}

void bn_trim(bn* a){
  while((a->len>0) && (a->n[a->len-1]==0))
    --a->len;
}

void bn_zero_left(bn* a, int len){
#ifdef pldbg
  if(BNSIZE < len) dbgmsg("bn_type bn_zero_left overflow error");
#endif
  while(a->len < len)
    a->n[a->len]=0;
}

void bn_shl(bn*a, int n){
  a->len+= n;
#ifdef pldbg
  if(BNSIZE < a->len) dbgmsg("bn_type bn_shl overflow error\n");
#endif
  int i= a->len-1;
  while(i>=n){
    a->n[i]= a->n[i-n];
    --i;
  }
  while(i>=0){
    a->n[i]=0;
    --i;
  }
}
  
void bn_shr(bn*a, int n){
  if(a->len <= n){
    bn_set(a,0);
    return;
  }
  a->len-= n;
  int i= 0;
  for(;i<a->len;++i)
    a->n[i]= a->n[i+n];
}

void bn_split(bn*from,int sp,bn*h,bn*l){
#ifdef pldbg
  if(from->len < sp) dbgmsg("bn_type bn_split split-out-of-range error");
#endif
  int i;
  for(i=0;i<from->len;++i){
    if(i<sp){
      l->n[i]= from->n[i];
    }else{
      h->n[i-sp]= from->n[i];
    }
  }
  l->len= sp;
  h->len= from->len - sp;
}

void bn_print(bn* a){
  int i= a->len;
  while(i-->0)
    printf("%08X",a->n[i]);
}

void bn_print_prefix(const char* pre,bn* a, const char* end){
  printf("%s[",pre);
  int i= a->len;
  while(i-->0)
    printf("%08X",a->n[i]);
  printf("]%s",end);
}

void bn_sinput(const char* str, bn* out){
  out->len=0;
  int32 tmp;
  while(sscanf(str,"%08x",&tmp)>0){
    str+=8;
    bn_shl(out,1);
    out->n[0]= tmp;
  }
}
int test_case_bn_sinput(){
  int ret=0;
  bn a;
  bn_sinput("12345678",&a);
  if(!(a.len==1 && a.n[0]==0x12345678))
    ++ret,printf("bn_type bn_sinput 12345678 !%08x len:%d\n",a.n[0],a.len);

  bn_sinput("123456789abcDEF0",&a);
  if(!(a.len==2 && a.n[1]==0x12345678 && a.n[0]==0x9abcdef0))
    ++ret,printf("bn_type bn_sinput 12345678 9abcDEF0 !%08x %08x len:%d\n",a.n[1],a.n[0],a.len);

  return ret;
}

int test_case_bn_type(){
  int ret=0;
  ret+= test_case_bn_sinput();
  return ret;
}


#endif
