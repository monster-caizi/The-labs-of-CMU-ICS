/* 
 * CS:APP Data Lab 
 * 
 * monster-caizi
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
/* 
 * bitAnd - x&y using only ~ and | 
 *   Example: bitAnd(6, 5) = 4
 *   Legal ops: ~ |
 *   Max ops: 8
 *   Rating: 1
 */
int bitAnd(int x, int y) {

  
  return ~((~x)|(~y));
}
/* 
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int getByte(int x, int n) {

  return (x>>(n<<3))&(0xFF);

}
/* 
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 0 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3 
 */
int logicalShift(int x, int n) {
  //  unsigned y=x;
  int mask = ((1 << 31) >> n) << 1;  // so smart method  https://github.com/scorix/coursera/blob/master/The%20Hardware%20Software%20Interface/lab1/bits.c
  return (x >> n) & ~mask;
  //  int mark = ((x>>n)^((1<<31)>>n<<1))&(x>>n);
  //return ((!!n)>>31<<31 & mark) | ((((!n)<<31)>>31)&x); 
 
}
/* 
 *  // Node by caizi that is the most exciting problem in unsigned number, prompted by https://github.com/gaocegege/ICS-Labs/blob/master/lab1/bits.c
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
int bitCount(int x) {
    int result = 0;
//    result = (x&(0x1))+(x&(0x2))+(x&(0x4))+(x&(0x8))+(x&(0x10))+(x&(0x20))+(x&(0x40))+(x&(0x80));//   
//    result = result + (x&(0x1)<<8)>>+(x&(0x2)<<8)+(x&(0x4)<<8)+(x&(0x8)<<8)+(x&(0x10)<<8)+(x&(0x20)<<8)+(x&(0x40)<<8)+(x&(0x80)<<8);//   
//    result = result + (x&(0x1)<<16)>>+(x&(0x2)<<16)+(x&(0x4)<<16)+(x&(0x8)<<16)+(x&(0x10)<<16)+(x&(0x20)<<16)+(x&(0x40)<<16)+(x&(0x80)<<16);//   
//    result = result + (x&(0x1)<<24)>>+(x&(0x2)<<24)+(x&(0x4)<<24)+(x&(0x8)<<24)+(x&(0x10)<<24)+(x&(0x20)<<24)+(x&(0x40)<<24)+(x&(0x80)<<24);//   
    int mark = (0x11<<8) | 0x11;
    mark = mark | (mark<<16);
    result = (x&mark) + ((x>>1)&mark) + ((x>>2)&mark) + ((x>>3)&mark);
    //printf("%x\n", result);
    mark = 0xff | (0xff<<8);
    result = (result>>16) + (result & mark) ;
    //printf("%x\n", result);
    result = (result>>12 & 0xf) + (result>>8 & 0xf) + (result>>4 & 0xf) + (result & 0xf) ;
    //printf("%x\n", result);
    return result;
}
/* 
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int bang(int x) {
  return (((~x+1)|x)>>31)+1; //-x|x guarantees that the signed bit is 1 and than shift 31 to get the number;
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
  return 0x1<<31;
}
/* 
 * fitsBits - return 1 if x can be represented as an 
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int fitsBits(int x, int n) {

    int y= (x>>31)^x;  // inverse
    return !(y>>(n+~1+1)); //trans to compare the positive number 

}
/* 
 * divpwr2 - Compute x/(2^n), for 0 <= n <= 30                  need to be checked.
 *  Round toward zero
 *   Examples: divpwr2(15,1) = 7, divpwr2(-33,4) = -2           Round to 0
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int divpwr2(int x, int n) {
    
    /*
   * For positive numbers, x/(2^n) == x >> n;
   * for negative numbers, x/(2@n) == (x + 1<<n - 1)>>n
   */
    //int lable = (((x>>31)&1)<<n)+(x>>31);
   // return  (x+lable)>>n;//(x+(((~(x>>31)+1)&!!n)<<(n+~1+1)))>>n ;
    
    int label = ((!!(x>>31))<<(n))+(x>>31);
    return (x+label)>>n;
    
    
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
    
  return ~x+1;
}
/* 
 * isPositive - return 1 if x > 0, return 0 otherwise 
 *   Example: isPositive(-1) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 3
 */
int isPositive(int x) {
      
    return !((x>>31|!x)&0x1);
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
    int z = (x^y)>>31&0x1;  // get the comparation of signed, when they are same, z=0, othewide z=1;
   // printf("x=%d  y=%d   %d \n",x,y,(!z&((x+(~y+1)+1)>>31)));
  return (!((0x1<<31)^x)) | (z&(x>>31)) | ((!z)&((x+(~y))>>31));// if z=1, x shold <0, else x-y should less or equal than 0. Besides, when x == to 0x80000000, the function is true whatever y is. 
}
/*
 * ilog2 - return floor(log base 2 of x), where x > 0
 *   Example: ilog2(16) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 90
 *   Rating: 4
 */
int ilog2(int x) {
    
    
    int mark = (0xff<<8) | 0xff;
    
    int sign = (!!(x & mark<<16))<<4;

    sign = ((!!(x>>sign & 0xff<<8))<<3) +sign;

    sign = ((!!(x>>sign & 0xf<<4))<<2) +sign;
 
    sign = ((!!(x>>sign & 0x3<<2))<<1) +sign;

    sign = ((!!(x>>sign & 0x1<<1))<<0)+sign;

  return sign;
}
/* 
 * float_neg - Return bit-level equivalent of expression -f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned float_neg(unsigned uf) {
    unsigned nuf = uf^(1<<31);
  //  printf("uf=%d\n",uf);
    if ((uf>0xff800000 && uf!=0xffffffff) || (nuf>0xff800000 && nuf!=0xffffffff ))
      return uf;
 return nuf;
}
/* 
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_i2f(int x) {
    int flag =x&(0x80000000), e=126, role,bias,frac,tailb;

    if(x == 0 )
        return 0;
    if(x == 0x80000000) 
        return 0xCF000000;
    if(x<0)
        x = -x;
    role=x;
    while(x>>(e-126))
    {
        e+=1;
    }
    // printf("e=%d  ",e);
    /*int f = 0x80000000;
     printf("x=%x  ",(~(f>>(180-e))));

    x= x & (~(f>>(180-e)));
      printf("ux=%x %x  ",x, (0x1<<(e-150)));
    if((x >(0x1<<(e-150))) || ((x==(0x1<<(e-150)))&&((role2>>(e-150))&0x1)))
        bias = 1;*/
    x = x << (158 - e); // clear all those zeros to the left of MSB, 31-e+127

    tailb = x & 0xFF; // byte to round

    bias = (tailb > 128) || ((tailb == 128) && ((x >> 8)  & 1)); // simply from frac = (x >> 8) & 0x7FFFFF &1
    frac = (e<<23);
    if(e<=150)
        return flag+frac+((role<<(150-e))^0x800000); 
    else
        return  flag+frac+((role>>(e-150))^0x800000)+bias;//need to rounding to even number when the discarded number ==0.5, else toward to the nearest number. 
}

/* 
 * float_twice - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_twice(unsigned uf) {
    unsigned nuf1 = uf^(1<<31);
     if ((uf>=0xff800000) || nuf1>=0xff800000 || uf ==0 ||nuf1 ==0)
      return uf;
    //printf("%x\n",!(uf&0x7f800000));
    if (!(uf&0x7f800000))
        return uf+(uf&0x7fffff);
    nuf1 = uf+0x00800000;
    /*unsigned nuf2 = nuf1^(1<<31);
    if (nuf1>0x7f800000 || nuf2>0x7f800000)
     return uf;*/
  return nuf1;
}    

