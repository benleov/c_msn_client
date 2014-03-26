#include "integers.h" 
#include <math.h> 


//1 + (int) (Math.log(i) / base10)
//where base10 = Math.log(10) 

int integerLength(int n)
{
        if (n == 0)
        {
                 return 1;
        }
        else
        {
        	double l = floor ( log10 ( n ) ) + 1; /* Number of digits in n */
        	return (int)l;
        } 
}
