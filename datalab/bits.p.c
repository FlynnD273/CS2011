#line 180 "bits.c"
int bitXor(int x, int y) {


  return ~(x & y) & ~(~x & ~y);
#line 31 "<command-line>"
#include "/usr/include/stdc-predef.h"
#line 184 "bits.c"
}
#line 191
int tmin(void) {
  return 1 << 31;
}
#line 202
int isTmax(int x) {
  int newx=  ~(x ^( x + 1));
  int allones=  !(~x);
  return !(newx | allones);
}
#line 215
int allOddBits(int x) {

  int evenbits=  85;
  evenbits = evenbits |( evenbits << 8);
  evenbits = evenbits |( evenbits << 16);


  x = x | evenbits;


  x = ~x;
  return !x;
}
#line 235
int negate(int x) {
  return 2;
}
#line 248
int isAsciiDigit(int x) {
  return 2;
}
#line 258
int conditional(int x, int y, int z) {
  return 2;
}
#line 268
int isLessOrEqual(int x, int y) {
  return 2;
}
#line 280
int logicalNeg(int x) {
  return 2;
}
#line 295
int howManyBits(int x) {
  return 0;
}
#line 310
unsigned floatScale2(unsigned uf) {
  int exp=  uf << 1 >> 24;
  int sign=  uf &( 1 << 31);


  if (!(uf & ~(1 << 31))) {
    return uf;
  }


  if (!exp) {
    return sign |( uf << 1);
  }


  if (exp == 0xFF) {
    return uf;
  }


  return sign |(( exp + 1) << 23) |( uf & ~(~0 >> 23 << 23));
}
#line 344
int floatFloat2Int(unsigned uf) {
  if (~uf >> 22 << 23) {

  }


  return 1 << 31;
}
#line 364
unsigned floatNegate(unsigned uf) {


  if (~uf >> 22 << 23) {
#line 371
    return (uf << 1 >> 1) |( ~uf &( 1 << 31));
  }


  return uf;
}
