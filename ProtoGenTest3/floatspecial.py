# floatspecial.py was generated by ProtoGen version 3.1.d

from floatspecial import *


from ctypes import *
from struct import *
from math   import *

def float32ToInt(value):
    """ Converts the bits of a float32 to type int with same bit pattern."""
    byteA = pack("f", value)
    integer = unpack("I", byteA)
    return integer[0]

def float64ToInt(value):
    """ Converts the bits of a float64 to type int with same bit pattern."""
    byteA = pack("d", value)
    integer = unpack("q", byteA)
    return integer[0]

def int32ToFloat(value):
    """ Converts the bits of a int32 to type float32 with same bit pattern."""
    byteA = pack("I", value)
    Float = unpack("f", byteA)
    return Float[0]

def int64ToFloat(value):
    """ Converts the bits of a int64 to type float64 with same bit pattern."""
    byteA = pack("q", value)
    Float = unpack("d", byteA)
    return Float[0]

def isFloat32Valid(value):
    """ Check if a 32-bit field is a valid 32-bit IEEE-754 float.

        Args:
            value (float32) the field to evaluate

        Returns: False if field is infinity, NaN, or de-normalized, else True

        Notes:
            # Five cases for floating point numbers:
            # 0) The exponent is > than zero and < than the maximum, a normal non-zero number.
            # 1) The exponent and the significand are zero. This is zero.
            # 2) The exponent is zero, and the significant is non-zero. This is denormalized.
            # 3) The exponent is the maximum value, and the significand is zero. This is infinity.
            # 4) The exponent is the maximum value, and the significand is non-zero. This is NaN.
            # We check for cases 2, 3, 4 and return 0 if it happens
    """

    #  if the exponent is the maximum value
    if (value & 0x7F800000) == (0x7F800000):
        # inifinity or NaN.
        return False

    # else if the exponent is zero
    elif (value & 0x7F800000) == 0:
        #Check for denormalized number
        if value & 0x007FFFFF:
            return False

    # If we get here then its a valid float
    return True

def isFloat64Valid(value):
    """ Check if a 64-bit field is a valid 64-bit IEEE-754 float.

        Args:
            value (float64) the field to evaluate

        Returns: False if field is infinity, NaN, or de-normalized, else True

        Notes:
            # Five cases for floating point numbers:
            # 0) The exponent is > than zero and < than the maximum, a normal non-zero number.
            # 1) The exponent and the significand are zero. This is zero.
            # 2) The exponent is zero, and the significant is non-zero. This is denormalized.
            # 3) The exponent is the maximum value, and the significand is zero. This is infinity.
            # 4) The exponent is the maximum value, and the significand is non-zero. This is NaN.
            # We check for cases 2, 3, 4 and return 0 if it happens
    """

    # if the exponent is the maximum value
    if (value & 0x7FF0000000000000) == (0x7FF0000000000000):
        # inifinity or NaN.
        return False

    # else if the exponent is zero
    elif (value & 0x7FF0000000000000) == 0:
        # Check for denormalized number
        if value & 0x000FFFFFFFFFFFFF:
            return False

    # If we get here then its a valid float
    return True

def float32ToFloat24(value, sigbits):
    """ Convert a 32-bit floating point value (IEEE-754 binary32) to 24-bit floating
        point representation with a variable number of bits for the significand.

        Args:
            value (32-bit float) the data to convert
            sigbits (int) the number of bits to use for the significand,
                it must bebetween 4 and 20 bits inclusive

        Returns: The float24 as a simple 24-bit integer

        Notes:
            Underflow will be returned as zero and overflow as the maximum possible value.
    """

    maxsignificand = c_uint((1 << sigbits) - 1)

    # The bias is computed as 2 raised to the number of exponent bits divided
    # by two, minus 1. This can be simplified as 2^(exponent bits -1) - 1
    # The number of exponent bits is 24 - 1 - sigbits
    bias = (1 << (22 - sigbits)) - 1

    # Pack it as a float and unpack it as an integer to get the same bit order
    # but of type int
    integer = float32ToInt(value)

    # The significand is the least significant 23 bits (IEEE754)
    significand = c_uint(integer & 0x007FFFFF)

    # Exponent occupies the next 8 bits (IEEE754)
    unsignedExponent = c_uint((integer & 0x7F800000) >> 23)

    # Get rid of some bits here is where we sacrifice resolution, so we need
    # to round correctly, for that reason we do not shift the last bit yet
    output = c_uint(significand.value >> (22-sigbits))

    # Check for rounding, if the most significant bit that we are going to
    # throw away is a 1 we need to round up.
    if output.value & 0x01:
        output.value = (output.value >> 1)

        # Don't round past the maximum significand
        if output.value < maxsignificand.value:
            output.value = output.value + 1
    else:
        output.value = (output.value >> 1)

    # If significand and exponent are zero means a number of zero
    if (output.value == 0) and (unsignedExponent.value == 0):
        # return correctly signed result
        if integer & 0x80000000:
            return 0x00800000
        else:
            return 0

    # Get the un-biased exponent. Binary32 is biased by 127
    signedExponent = unsignedExponent.value - 127

    if signedExponent < -bias:
        output = 0   # underflow to zero
    else:
        if signedExponent > bias:
            # Largest possible exponent and significand without making a NaN or Inf
            signedExponent = bias
            output.value = (1 << sigbits) - 1

        # re-bias with the new bias
        unsignedExponent.value = signedExponent + bias

        # Put the exponent in the output
        output.value = output.value | (unsignedExponent.value << sigbits)

    # Account for the sign
    if integer & 0x80000000:
        output.value = output.value | 0x00800000

    # return the 24-bit representation
    return output.value

def float24ToFloat32(value, sigbits):
    """ Convert a 24-bit floating point representation with variable number of
        significand bits to binary32

        Args:
            value (uint32) the 24-bit floating point representation to convert
            sigbits (int) the number of bits to use for the significand,
                it must bebetween 4 and 20 bits inclusive

        Returns: a float32 of the data
    """

    # Zero is a special case
    if (value & 0x007FFFFF) == 0:
        Integer = 0
    else:
        # The mask for the significand bits
        sigmask = (1 << sigbits) - 1

        # The unsigned exponent, mask off the leading sign bit
        unsignedExponent = c_uint(((value & 0x007FFFFF) >> sigbits))

        # The bias is computed as 2 raised to the number of exponent bits divided
        # by two, minus 1. This can be simplified as 2^(exponent bits -1) - 1
        # The number of exponent bits is 24 - 1 - sigbits
        bias = (1 << (22 - sigbits)) - 1

        # We want to subtract our bias to get un-biased, and then add 127 for the new bias
        unsignedExponent.value += (127 - bias)

        # Reduced bits of signficand, shift it up to 23 bits
        Integer = (value & sigmask) << (23-sigbits)

        # Put the exponent in
        Integer |= (unsignedExponent.value << 23)

    # And the sign bit
    if value & 0x00800000:
        Integer |= 0x80000000

    return int32ToFloat(Integer)

def float32ToFloat16(value, sigbits):
    """ Convert a 32-bit floating point value (IEEE-754 binary32) to 16-bit floating
        point representation with a variable number of bits for the significand

        Args:
            value (float32) the number to convert
            sigbits (int) the number of bits to use for the significand,
                it must bebetween 4 and 12 bits inclusive

        Returns: float16 as a simple 16-bit integer
    """

    maxsignificand = c_ushort(((1 << sigbits) - 1))

    # The bias is computed as 2 raised to the number of exponent bits divided
    # by two, minus 1. This can be simplified as 2^(exponent bits -1) - 1
    # The number of exponent bits is 16 - 1 - sigbits
    bias = (1 << (14 - sigbits)) - 1;

    # Pack it as a float and unpack it as an integer to get the same bit order
    # but of type int
    integer = float32ToInt(value)

    # The significand is the least significant 23 bits (IEEE754)
    significand = c_uint(integer & 0x007FFFFF)

    # Exponent occupies the next 8 bits (IEEE754)
    unsignedExponent = c_uint((integer & 0x7F800000) >> 23)

    # Get rid of some bits; here is where we sacrifice resolution, so we need
    # to round correctly, for that reason we do not shift the last bit yet
    output = c_ushort((significand.value >> (22 - sigbits)))

    # Check for rounding, if the most significant bit that we are going to
    # throw away is a 1 we need to round up.
    if output.value & 0x01:
        output.value = (output.value >> 1)

        # Don't round past the maximum significand
        if output.value < maxsignificand.value:
            output.value = output.value + 1
    else:
        output.value = (output.value >> 1)

    # If significand and exponent are zero means a number of zero
    if (output.value == 0) and (unsignedExponent.value == 0):
        # return correctly signed result
        if integer & 0x80000000:
            return 0x8000
        else:
            return 0

    # Get the un-biased exponent. Binary32 is biased by 127
    signedExponent = unsignedExponent.value - 127

    # With a 6-bit exponent we can support exponents of
    # exponent : biased value
    # -31      : 0 (value is zero or denormalized)
    # -30      : 1
    #  -1      : 30
    #   0      : 31
    #   1      : 32
    #  31      : 62
    #  32      : NaN (all exponent bits are 1)

    # With a 5-bit exponent we get
    # exponent : biased value
    # -15      : 0 (value is zero or denormalized)
    # -14      : 1
    #  -1      : 14
    #   0      : 15
    #   1      : 16
    #  15      : 32
    #  16      : NaN (all exponent bits are 1)

    if signedExponent < -bias:
        output.value = 0   # underflow to zero
    else:

        if signedExponent > bias:
            # Largest possible exponent and significand without making a NaN or Inf
            signedExponent = bias
            output.value = (1 << sigbits) - 1

        # re-bias with the new bias
        unsignedExponent.value = signedExponent + bias

        # Put the exponent in the output
        output.value = output.value | (unsignedExponent.value << sigbits)

    # Account for the sign
    if integer & 0x80000000:
        output.value = output.value | 0x8000

    # return the binary16 representation
    return output.value

def float16ToFloat32(value, sigbits):
    """ Convert a 16-bit floating point representation with variable number of
        significand bits to binary32

        Args:
            value (uint32) the float16 representation to convert
            sigbits (int) the number of bits to use for the significand,
                it must bebetween 4 and 12 bits inclusive

        Returns: float32 from encoded data
    """

    # Zero is a special case
    if (value & 0x7FFF) == 0:
        Integer = 0

    else:
        # The mask for the significand bits
        sigmask = (1 << sigbits) - 1

        # The unsigned exponent, mask off the leading sign bit
        unsignedExponent = c_uint(((value & 0x7FFF) >> sigbits))

        # The bias is computed as 2 raised to the number of exponent bits divided
        # by two, minus 1. This can be simplified as 2^(exponent bits -1) - 1
        # The number of exponent bits is 16 - 1 - sigbits
        bias = (1 << (14 - sigbits)) - 1

        # We want to subtract our bias to get un-biased, and then add 127 for the new bias
        unsignedExponent.value += (127 - bias)

        # Reduced bits of signficand, shift it up to 23 bits
        Integer = (value & sigmask) << (23-sigbits)

        # Put the exponent in
        Integer = Integer | (unsignedExponent.value << 23)

    # And the sign bit
    if value & 0x8000:
        Integer = Integer | 0x80000000

    return int32ToFloat(Integer)


def testSpecialFloat():
    """Use this routine (and a debugger) to verify the special float functionality.

       Returns: True or false if passes
    """

    dataIn    = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
    dataOut16 = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
    dataOut24 = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0]

    error = 0.0

    value   = -.123456789
    Integer = float32ToInt(value)


    for i in range(3):
        value = value * 10.0
        dataIn[i] = value
        dataOut16[i] = float16ToFloat32(float32ToFloat16(dataIn[i], 9), 9)
        dataOut24[i] = float24ToFloat32(float32ToFloat24(dataIn[i], 15), 15)
        error += fabs((dataIn[i] - dataOut16[i])/dataIn[i])
        error += fabs((dataIn[i] - dataOut24[i])/dataIn[i])

    value = 12.3456789
    for i in range(6):
        value /= 10.0
        dataIn[i] = value
        dataOut16[i] = float16ToFloat32(float32ToFloat16(dataIn[i], 9), 9)
        dataOut24[i] = float24ToFloat32(float32ToFloat24(dataIn[i], 15), 15)
        error += fabs((dataIn[i] - dataOut16[i])/dataIn[i])
        error += fabs((dataIn[i] - dataOut24[i])/dataIn[i])

    if error > 0.01:
        return False

    # Test rounding
    value = float16ToFloat32(float32ToFloat16(33.34, 10), 10)
    if value < 33.34:
        return False

    value = float16ToFloat32(float32ToFloat16(33.32, 10), 10)
    if value > 33.32:
        return False

    # Maximum possible float without Inf or Nan
    Integer = 0x7F7FFFFF

    # This loop exercises the overflow and underflow, use the debugger to verify functionality
    i = 0
    for i in range(6):
        dataIn[i] = Float
        dataOut16[i] = float16ToFloat32(float32ToFloat16(dataIn[i], 9), 9)
        dataOut24[i] = float24ToFloat32(float32ToFloat24(dataIn[i], 15), 15)
        Float = Float / 1000000000000.0

    return True
