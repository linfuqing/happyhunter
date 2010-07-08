#ifndef BASICMATH_H
#define BASICMATH_H

#include "datatype.h"
#include "Debug.h"

///
// 基本的数学函数
///
namespace zerO
{
#define MAX(x, y) ( (x) > (y) ? (x) : (y) )
#define MIN(x, y) ( (x) < (y) ? (x) : (y) )
#define ROUND(x) (INT)( (x) > 0.0f ? ( (x) + 0.5f ) : ( (x) < 0.0f ? (x) : 0.0f ) )
#define FLOOR(x) (INT)( (x) > 0.0f ? (x) : ( (x) < 0.0f ? ( (x) - 0.5f ) : 0.0f ) )
#define CLAMP(Value, Min, Max) ( (Value) > (Max) ? (Max) : ( (Value) < (Min) ? (Min) : (Value) ) )

	#define fpBits(f) (*reinterpret_cast<const INT*>(&(f))) 

// reinterpret an int32 as a float
#define intBits(i) (*reinterpret_cast<const float*>(&(i))) 

// return 0 or -1 based on the sign of the float
#define fpSign(f) (fpBits(f)>>31) 

// extract the 8 bits of exponent as a signed integer
// by masking out this bits, shifting down by 23,
// and subtracting the bias value of 127
#define fpExponent(f) (((fpBits(f)&0x7fffffff)>>23)-127) 

// return 0 or -1 based on the sign of the exponent
#define fpExponentSign(f) (fpExponent(f)>>31) 

// get the 23 bits of mantissa without the implied bit
#define fpPureMantissa(f) ((fpBits(f)&0x7fffff)) 

// get the 23 bits of mantissa with the implied bit replaced
#define fpMantissa(f) (fpPureMantissa(f) | (1<<23)) 

#define fpOneBits 0x3F800000

// flipSign is a helper Macro to
// invert the sign of i if flip equals -1, 
// if flip equals 0, it does nothing
#define flipSign(i, flip) ((i^ flip) - flip)

	inline INT realToInt32_floor(float input)
	{ 
		// read the exponent and decide how much we need to shift the mantissa down
		int shift = 23-fpExponent(input);
		// read the mantissa and shift it down to remove all fractional values
		int result = fpMantissa(input)>>shift;
		// set the sign of the new result
		result = flipSign(result, fpSign(input));
		// if the exponent was negative, (-1<input<1) we must return zero
		result &= ~fpExponentSign(input);

		// if the original value is negative, and any fractional values are present,
		// decrement the result by one
		result -= fpSign(input) && (fpExponentSign(input) || (fpPureMantissa(input) & ((1<<shift)-1)));

		// return the result
		return result;                  
	}
}

#endif