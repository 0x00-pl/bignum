#ifndef BN_MUL
#define BN_MUL

#include "bn_add.h"

void bn_mul_base(bn*a,bn*b,bn*out){
  bn_trim(a);
  bn_trim(b);
  int i,j;
  int64 tmp;
  out->len= a->len+b->len;
  for(i=0; i<out->len; ++i) out->n[i]=0;
  for(i=0; i<a->len; ++i){
    tmp=0;
    for(j=0; j<b->len; ++j){
      tmp= out->n[i+j] + (int64)a->n[i]*b->n[j] + tmp;
      out->n[i+j]= tmp&0xffffffff;
      tmp= tmp>>32;
    }
    out->n[i+b->len]= tmp;
  }
  bn_trim(out);
}
void bn_mul(bn*a,bn*b,bn*out);
void bn_mul_karatsuba(bn*a,bn*b,bn*out){
  bn t1,t2,t3,t4,t5,t6;
  bn*ah,*al,*bh,*bl,*z0,*z1,*z2;
  ah=&t1;
  al=&t2;
  bh=&t3;
  bl=&t4;
  z0=&t5;
  //z1=ah; z1 can use mem of ah, init after 
  z2=&t6;
  int sp;
  if(a->len > b->len){
    sp= a->len/2;
    bn_split(a,sp,ah,al);
    if(b->len < sp){
      bn_set(bh,0);
      bn_copy(b,bl);
    }else{
      bn_split(b,sp,bh,bl);
    }
  }else{
    sp= b->len/2;
    bn_split(b,sp,bh,bl);
    if(a->len < sp){
      bn_set(ah,0);
      bn_copy(a,al);
    }else{
      bn_split(a,sp,ah,al);
    }
  }
  //z0= al*bl
  bn_mul(al,bl,z0);
  //z2= ah*bh
  bn_mul(ah,bh,z2);
  //z1= (ah+al)(bh+bl)-z0-z2
  z1=ah;//ah is never use after
  bn_add(ah,al,al);//al,bl is never use after
  bn_add(bh,bl,bl);
  bn_mul(al,bl,z1);
  bn_sub(z1,z0,z1);
  bn_sub(z1,z2,z1);
  //ret= z2<<sp*2 + z1<<sp + z0
  bn_copy(z2,out);
  bn_shl(out,sp);
  bn_add(out,z1,out);
  bn_shl(out,sp);
  bn_add(out,z0,out);
  bn_trim(out);
#ifdef pldbg
  bn test;
  bn_mul_base(a,b,&test);
  if(bn_cmp(&test,out)!=0){
    dbgmsg("bn_mul bn_mul_karatsuba  error\n");
    bn_print_prefix("y: ",&test,"\n");
    bn_print_prefix("n: ",out,"\n\n");
  }
#endif
}
void bn_mul(bn*a,bn*b,bn*out){
#ifdef pldbg
  if(a->len+b->len > BNSIZE) dbgmsg("bn_mul bn_mul overflow error\n");
#endif
  if(a->len*b->len >= 256){
    bn_mul_karatsuba(a,b,out);
  }else{
    if(a->len < b->len)
      bn_mul_base(b,a,out);
    else
      bn_mul_base(a,b,out);
  }
}
int test_case_bn_mul_(){
  int ret=0;
  bn a,b,c,r;
  bn_sinput("99999999",&a);
  bn_sinput("00000000",&b);
  bn_sinput("00000000",&c);
  bn_trim(&c);
  bn_mul(&a,&b,&r);
  if(bn_cmp(&r,&c)) 
    ++ret, bn_print_prefix("bn_add bn_mul 99999999*0==0 !",&r,"\n");
 
  bn_sinput("99999999",&a);
  bn_sinput("10000000",&b);
  bn_sinput("0999999990000000",&c);
  bn_mul(&a,&b,&r);
  if(bn_cmp(&r,&c)) 
    ++ret, bn_print_prefix("bn_add bn_mul 99999999*100000000==0999999990000000 !",&r,"\n");

  bn_sinput("0000001111111111",&a);
  bn_sinput("0000001111111111",&b);
  bn_sinput("00000123456789a987654321",&c);
  bn_mul(&a,&b,&r);
  if(bn_cmp(&r,&c)) 
    ++ret, bn_print_prefix("bn_add bn_mul 1111111111*1111111111==00000123456789a987654321 !",&r,"\n");
  
  return ret;
}
void bn_mod_old(bn*a,bn*b,bn*out){
  bn_trim(a);
  bn_trim(b);
  bn sn;
  bn sbt;
  int64 tmp=0;
  bn_copy(a,out);
  while(out->len > b->len){
    //a%b ===> a1xxx%b1xxx ==> (a-(a1/b1)*b)%b
    //sn= a1/b1
    if(b->n[b->len-1]!=0xffffffff){
      bn_copy(out,&sbt);bn_shr(&sbt, b->len-1);
      bn_div_int(&sbt,b->n[b->len-1]+1,&sn);
    }else{
      bn_copy(out,&sn); bn_shr(&sn, b->len);
    }
    //sn=b*sn
    bn_copy(&sn,&sbt); bn_mul(b,&sbt,&sn);
    //out=out-sn
    bn_sub(out,&sn,out);
  }
  while(bn_cmp(out,b)>=0){
    bn_sub(out,b,out);
    if(bn_cmp(out,b)<0) break;
    if(b->n[b->len-1]!=0xffffffff){
      tmp= out->n[out->len-1] / (b->n[b->len-1]+1);
      //sn=b*tmp
      bn_copy(b,&sn);bn_mul_int(&sn,tmp,&sn);
      //out=out-sn
      bn_sub(out,&sn,out);
    }
    bn_trim(out);
  }
  }
void bn_mod(bn*a,bn*m,bn*out){
  int sign=0;
  bn t1;
  bn *pa=out,*nm=&t1;
  bn_copy(a,pa);
  while(1){
    /*bn_print_prefix("pa: ",pa,"\n");
    bn_print_prefix("m : ",m,"\n");
    sleep(1);*/
    if(bn_cmp(pa,m)<0){
      if(sign){
	bn_sub(m,pa,out);
	break;
      }else{
	if(pa!=out)
	  bn_copy(pa,out);
	break;
      }
    }
    //nm ~~= pa/m 
    int64 n;
    if(pa->len-m->len!=0){
      n= ((int64)pa->n[pa->len-1]<<32) | pa->n[pa->len-2];
      n= n / m->n[m->len-1];
      bn_mul_int64(m,n,nm);
      bn_shl(nm, pa->len-m->len-1);
    }else{
      n= pa->n[pa->len-1] / m->n[m->len-1];
      bn_mul_int(m,n,nm);
    }
    //pa=pa-nm
    int cp= bn_cmp(pa,nm);
    if(cp<0){
      sign=~sign;
      bn_sub(nm,pa,pa);
    }else{
      bn_sub(pa,nm,pa);
    }
  }
  bn_trim(out);
#ifdef pldbg
  bn_mod_old(a,m,&t1);
  if(bn_cmp(&t1,out)!=0){
    dbgmsg("bn_mul bn_mod fault\n");
    bn_print_prefix("",a,"  mod\n");
    bn_print_prefix("",m,"=\n");
    bn_print_prefix("y:",&t1,"\n");
    bn_print_prefix("n:",out,"\n\n");
    sleep(3);
  }  
#endif
}
int test_case_bn_mod(){
  int ret=0;
  bn a,b,c,r;
  bn_sinput("99999999",&a);
  bn_sinput("00000100",&b);
  bn_sinput("00000099",&c);
  bn_mod(&a,&b,&r);
  if(bn_cmp(&r,&c)) 
    ++ret, bn_print_prefix("bn_add bn_mod 99999999 mod 100==99 !",&r,"\n");
 
  bn_sinput("9999999999999999",&a);
  bn_sinput("10000000",&b);
  bn_sinput("09999999",&c);
  bn_mod(&a,&b,&r);
  if(bn_cmp(&r,&c)) 
    ++ret, bn_print_prefix("bn_add bn_mod 9999999999999999 mod 10000000==09999999 !",&r,"\n");

  bn_sinput("00000123456789a987654322",&a);
  bn_sinput("0000001111111111",&b);
  bn_sinput("00000001",&c);
  bn_mod(&a,&b,&r);
  if(bn_cmp(&r,&c)) 
    ++ret, bn_print_prefix("bn_add bn_mod 00000123456789a987654322 mod 0000001111111111==1 !",&r,"\n");
  
  return ret;
}

void bn_div(bn*a,bn*b,bn*out){
  bn_trim(a);
  bn_trim(b);
  bn sn;
  bn sbt;
  bn sa;
  int64 tmp=0;
  bn_set(out,0);
  bn_copy(a,&sa);
  while(sa.len > b->len){
    //a%b ===> a1xxx%b1xxx ==> (a-(a1/b1)*b)%b
    //sn= a1/b1
    if(b->n[b->len-1]!=0xffffffff){
      bn_copy(&sa,&sbt);bn_shr(&sbt, b->len-1);
      bn_div_int(&sbt,b->n[b->len-1]+1,&sn);
    }else{
      bn_copy(&sa,&sn); bn_shr(&sn, b->len);
    }
    bn_add(&sn,out,out);
    //sn=b*sn
    bn_copy(&sn, &sbt); bn_mul(b, &sbt, &sn);
    //sa=sa-sn
    bn_sub(&sa, &sn, &sa);
  }
  while(bn_cmp(&sa,b)>=0){
    bn_sub(&sa,b,&sa);
    bn_add_int(out,1);
    if(bn_cmp(&sa,b)<0) break;
    tmp= (int64)sa.n[sa.len-1] / (b->n[b->len-1]+1);
    bn_add_int(out,tmp);
    //sn=b*tmp
    bn_copy(b,&sn);bn_mul_int(&sn,tmp,&sn);
    //sa=sa-sn
    bn_sub(&sa,&sn,&sa);
  }
  bn_trim(out);
}
int test_case_bn_div(){
  int ret=0;
  bn a,b,c,r;
  bn_sinput("00000000",&a);
  bn_sinput("99999999",&b);
  bn_sinput("00000000",&c);
  bn_trim(&c);
  bn_div(&a,&b,&r);
  if(bn_cmp(&r,&c)) 
    ++ret, bn_print_prefix("bn_add bn_div 0/99999999==0 !",&r,"\n");
 
  bn_sinput("0000000990000000",&a);
  bn_sinput("10000000",&b);
  bn_sinput("00000099",&c);
  bn_div(&a,&b,&r);
  if(bn_cmp(&r,&c)) 
    ++ret, bn_print_prefix("bn_add bn_div 0000000990000000/100000000==99 !",&r,"\n");

  bn_sinput("00000123456789a987654321",&a);
  bn_sinput("0000001111111111",&b);
  bn_sinput("0000001111111111",&c);
  bn_div(&a,&b,&r);
  if(bn_cmp(&r,&c)) 
    ++ret, bn_print_prefix("bn_add bn_div 00000123456789a987654321/1111111111==0000001111111111 !",&r,"\n");
  
  return ret;
}
void bn_mul_mod_old(bn*a,bn*b,bn*m,bn*out){
  bn sdlt;
  int i;
  bn_set(out,0);
  for(i=b->len-1; i>=0; --i){
    bn_mul_int(a, b->n[i],&sdlt);
    bn_shl(out,1);
    bn_add(&sdlt, out, &sdlt);
    bn_mod(&sdlt, m, out);
  }
}

void bn_mul_mod(bn*a,bn*b,bn*m,bn*out){
  bn t1;
  bn_mul(a,b,&t1);
  bn_mod(&t1,m,out);
}

int test_case_bn_mul_mod(){
  int ret=0;
  bn a,b,m,c,r;
  bn_sinput("0000001111111111",&a);
  bn_sinput("0000001111111111",&b);
  bn_sinput("abababab",&m);
  bn_sinput("12121333",&c);
  bn_mul_mod(&a,&b,&m,&r);
  if(bn_cmp(&r,&c)) 
    ++ret, bn_print_prefix("bn_add bn_div 12121333 !",&r,"\n");
 
  return ret;
}
void bn_sqr_mod_old(bn*a,bn*m,bn*out){
  //bn_mul_mod(a,a,m,out); return;//dbg
  bn t1;
  bn*o=out,*o2=&t1;
  int64 sum,tmul;
  int i,k;
  bn_set(o,0);
  for(k=(a->len-1)*2; k>=0; --k){
    sum=0;
    for(i=k/2+1; i<=k&&i<a->len; ++i){
      tmul= (int64)a->n[i]*a->n[k-i];
      sum+= ((tmul<<1)&0xffffffff);
      bn_add_int64(o, (int64)(sum>>32)+(tmul>>31));
      sum&= 0xffffffff;
    }
    tmul=0;
    if((k&1)==0){
      tmul= (int64)a->n[k/2]*a->n[k/2];
    }
    bn_add_int(o,tmul>>32);
    sum+=(tmul&0xffffffff);
    bn_mod(o,m,o2);
    bn*tmp=o2;o2=o;o=tmp;
    bn_add_int(o,sum>>32);
    bn_shl(o,1);
    bn_add_int(o,sum&0xffffffff);
  }

  bn_mod(o,m,o2);
  bn*tmp=o2;o2=o;o=tmp;
  if(o!=out)
    bn_copy(o,out);
}
void bn_sqr(bn*a,bn*out){
  //bn_mul_mod(a,a,m,out); return;//dbg
  bn t1;
  int64 sumh,suml,tmul;
  int i,k;
  bn_set(out,0);
  sumh=0;
  for(k=(a->len-1)*2; k>=0; --k){
    suml= 0;
    for(i=k/2+1; i<=k&&i<a->len; ++i){
      tmul= (int64)a->n[i]*a->n[k-i];
      suml+= ((tmul<<1)&0xffffffff);
      sumh+= (suml>>32)+(tmul>>31);
      suml&= 0xffffffff;
    }
    suml+= ((int64)a->n[k/2]*a->n[k/2])&(((int64)(k&1))-1);
    sumh+= (suml>>32);
    bn_add_int64(out,sumh);
    bn_shl(out,1);
    sumh= suml&0xffffffff;
  }
  bn_add_int(out,sumh&0xffffffff);
}
void bn_sqr_mod(bn*a,bn*m,bn*out){
  bn t1;
  bn_sqr(a,&t1);
  bn_mod(&t1,m,out);
}
int test_case_bn_sqr_mod(){
  int ret=0;
  bn a,m,c,r;
  bn_sinput("11111111",&a);
  bn_sinput("00000100",&m);
  bn_sinput("00000021",&c);
  bn_sqr_mod(&a,&m,&r);
  if(bn_cmp(&r,&c)) 
    ++ret, bn_print_prefix("bn_mul bn_sqr_mod 21 !",&r,"\n");

  bn_sinput("0000001111111111",&a);
  bn_sinput("abababab",&m);
  bn_sinput("12121333",&c);
  bn_sqr_mod(&a,&m,&r);
  if(bn_cmp(&r,&c)) 
    ++ret, bn_print_prefix("bn_mul bn_sqr_mod 12121333 !",&r,"\n");

  bn_sinput("45CF447C19414DAC919039FD0A4881C5",&a);
  bn_sinput("AB78A8A36D5B09E62314EA6AE0090C4D",&m);
  bn_sinput("3EF06F01E79A3CB659FEA388EE3C7280",&c);
  bn_sqr_mod(&a,&m,&r);
  if(bn_cmp(&r,&c)) 
    ++ret, bn_print_prefix("bn_mul bn_sqr_mod \n 13F2F6752A4785AF3E067451363544B1 !\n",&r,"\n");
 
  return ret;
}

int test_case_bn_mul(){
  int ret=0;
  ret+= test_case_bn_mul_();
  ret+= test_case_bn_mod();
  ret+= test_case_bn_div();
  ret+= test_case_bn_mul_mod();
  ret+= test_case_bn_sqr_mod();
  return ret;
}


#endif
