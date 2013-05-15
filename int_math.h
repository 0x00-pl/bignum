#ifndef INT_MATH
#define INT_MATH

#include "bn_type.h"

void int_add(int32 a, int32 b, int32* oh, int32* ol){
  int64 tmp= (int64)a+b;
  *ol= tmp&0xffffffff;
  *oh= tmp>>32;
}
int test_case_int_add(){
  int ret=0;
  int32 h,l;
  int_add(0,0,&h,&l);
  if(!(h==0 && l==0)) ret+=1,printf("int_add 0+0==0 !%08x%08x\n",h,l);
  
  int_add(0xffffffff,1,&h,&l);
  if(!(h==1 && l==0)) 
    ret+=1,printf("int_add 0xffffffff+1==0x100000000 !%08x%08x\n",h,l);
  
  int_add(0xffffffff,0xffffffff,&h,&l);
  if(!(h==1 && l==0xfffffffe))
    ret+=1,printf("int_add 0xffffffff+0xffffffff==0x1fffffffe !%08x%08x\n",h,l);
  return ret;
}

void int_sub(int32 a, int32 b, int32* oh, int32* ol){
  int64 tmp= (int64)1<<32;
  tmp= tmp+a-b;
  *ol= tmp&0xffffffff;
  *oh= 1-(tmp>>32);
}
int test_case_int_sub(){
  int ret=0;
  int32 h,l;
  int_sub(0,0,&h,&l);
  if(!(h==0 && l==0)) ret+=1,printf("int_sub 0-0== 0,0 !%08x,%08x\n",h,l);
  
  int_sub(0,1,&h,&l);
  if(!(h==1 && l==0xffffffff)) 
    ret+=1,printf("int_sub 0x00000000-1== 1,0xffffffff !%08x,%08x\n",h,l);
  
  int_sub(0xff,0xffffffff,&h,&l);
  if(!(h==1 && l==0x100))
    ret+=1,printf("int_sub 0xff-0xffffffff== 1,0x100 !%08x,%08x\n",h,l);
  return ret;
}

void int_mul(int32 a, int32 b, int32* oh, int32* ol){
  int64 tmp= (int64)a*b;
  *ol= tmp&0xffffffff;
  *oh= tmp>>32;
}
int test_case_int_mul(){
  int ret=0;
  int32 h,l;
  int_mul(0,0,&h,&l);
  if(!(h==0 && l==0)) ret+=1,printf("int_mul 0*0== 0 !%08x%08x\n",h,l);
  
  int_mul(0x11111111,0x11111111,&h,&l);
  if(!(h==0x1234567 && l==0x87654321)) 
    ret+=1,printf("int_mul 0x11111111*0x11111111== 0x123456787654321 !%08x%08x\n",h,l);
  
  int_mul(0xffffffff,0xffffffff,&h,&l);
  if(!(h==0xfffffffe && l==1))
    ret+=1,printf("int_mul 0xffffffff*0xffffffff== 0xfffffffe00000001 !%08x%08x\n",h,l);
  return ret;
}

void int_div(int64 a, int32 b, int32* oh, int32* ol){
  int64 tmp= (int64)a/b;
  *ol= tmp&0xffffffff;
  *oh= tmp>>32;
}
int test_case_int_div(){
  int ret=0;
  int32 h,l;
  int_div(0,1,&h,&l);
  if(!(h==0 && l==0)) ret+=1,printf("int_div 0/1== 0 !%08x%08x\n",h,l);
  
  int_div(0x123456787654321,0x11111111,&h,&l);
  if(!(h==0 && l==0x11111111)) 
    ret+=1,printf("int_div 0x123456787654321/0x11111111== 0x11111111 !%08x%08x\n",h,l);
  
  int_div(0xffffffff,0xffffffff,&h,&l);
  if(!(h==0 && l==1))
    ret+=1,printf("int_div 0xffffffff/0xffffffff== 1 !%08x%08x\n",h,l);
  return ret;
}

int test_case_int_math(){
  int ret=0;
  ret+= test_case_int_add();
  ret+= test_case_int_sub();
  ret+= test_case_int_mul();
  ret+= test_case_int_div();
  return ret;
}
#endif
