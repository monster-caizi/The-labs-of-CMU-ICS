#include <stdio.h>
typedef struct{
int a[2];
double d;
}struct_t;

double MemoryReferenceError(int i){
volatile struct_t s;
s.d = 3.14;
s.a[i] = 1073741824;
return s.d;
}

int main()
{

    /*//1. Memory reference error test
    printf("0 %.13f\n",MemoryReferenceError(0));
    printf("1 %.13f\n",MemoryReferenceError(1));
    printf("2 %.13f\n",MemoryReferenceError(2));
    printf("3 %.13f\n",MemoryReferenceError(3));
    printf("4 %.13f\n",MemoryReferenceError(4));
    printf("5 %.13f\n",MemoryReferenceError(5));
    printf("6 %.13f\n",MemoryReferenceError(6));*/
    /*// Test of right shitf more than the type, being not defined in pirticul system
    char a = 'a';
    a = a<<9;
    printf("%d\n",a);//return 0 when a == 'a'
    a = 'b';
    a = a<<-1;
    printf("%d\n",a);//return 0 when a == 'b' and shift -1*/

    //2. an unsigned number compares with a singned number, both number will be seen as unsigned numbers
    
    /*//3. test of mines Tmin, because -a == (inverse a) + 1 in binary calculate. 
    int x = -1<<31;
    printf("%d\n",x);
    if (x <0)
        printf("%d\n",-x);//the output is same, -2147483648
    else
        printf("%d\n",x); */
    
    //4. Umax = 2*Tmax+1

    /* //5. it won't stop when i is unsigned
    unsigned i;
    for(i = n-1;i>=0;i--)  //remember that for(i= n-2; i<n; i++) can stop
        f(i);
    // it won't stop neighter, because sizeof return a unsigned number 
    int i;
    for(i = n-1;i - sizeof(char) >=0;i--)
        f(i);
    */

    //6. be careful of sign short to int cast

    /*//7. test of the output of negative number calculate compare with unsigned number
    //both output 2
    char a = -1, b = -2;

    printf("%d\n",a*b);
    
    char c = 255, d = 254;
    
    printf("%d\n",c*d);
    //both output -2
    a = -1, b = 2;

    printf("%d\n",a*b);
    
    c = 255, d = 2;
    
    printf("%d\n",c*d);*/

    /*//8. u*(2^k) == u <<k; u/(2^k) == u>>k, besides, and when u is negative, u needs to plus 1; ; 
    printf("%d\n",-6>>1);
    printf("%d\n",(-6+1)>>1);
    printf("%d\n",-3>>1);
    printf("%d\n",(-3+1)>>1); */

    //9. x-> -x, please inverse x and plus 1    

    //10. No mater signed or unsigned number, addition or multiplication are the same
    // and negative number works out if it don't have a overflow. one type of overfolw for unsigned number and two for signed number
    // 

    //11. For Java, >>> for logical shift while >> for arithmetic shift

    //12. The address that allows to be used in 64-bin machine is 47 bits.

    //13. (x|-x)>>31 == -1 is false when x=0;

    //14. The expression of 1/3 in computer is the opposite of short division(divide 2), 
    //which using the method of mulitplying 2,is the number is bigger than 1, the bit
    //is noded as 1 else is 0.   https://www.zhihu.com/question/27908137

    //15. In the expression of float, 
    //   0 <= Exp <= 255
    //   -127<= E <= 128, different form two complement number

 return 0;

}

