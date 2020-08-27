# fieldencode.py was generated by ProtoGen version 3.1.d

from fieldencode import *
from struct import *
import sys	# regular

# supporting 64 bit sizes

def uint64ToBeBytes(byteA, index, number):
    """Encode a unsigned 8 byte integer on a big endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream

    Returns:
        index (int): The incremented index increased by 8
    """
    if number > 18446744073709551615:
        number = 18446744073709551615
    if number < 0:
        number = 0

    pack_into('>Q', byteA, index, number)
    index += 8
    return index

def uint64ToLeBytes(byteA, index, number):
    """Encode a unsigned 8 byte integer on a little endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream

    Returns:
        index (int): The incremented index increased by 8
    """
    if number > 18446744073709551615:
        number = 18446744073709551615
    if number < 0:
        number = 0

    pack_into('<Q', byteA, index, number)
    index += 8
    return index

def int64ToBeBytes(byteA, index, number):
    """Encode a signed 8 byte integer on a big endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream

    Returns:
        index (int): The incremented index increased by 8
    """
    if number > sys.maxsize:
        number = sys.maxsize
    if number < (-sys.maxsize):
        number = (-sys.maxsize)

    pack_into('>q', byteA, index, number)
    index += 8
    return index

def int64ToLeBytes(byteA, index, number):
    """Encode a signed 8 byte integer on a little endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream

    Returns:
        index (int): The incremented index increased by 8
    """
    if number > sys.maxsize:
        number = sys.maxsize
    if number < (-sys.maxsize):
        number = (-sys.maxsize)

    pack_into('<q', byteA, index, number)
    index += 8
    return index

def uint56ToBeBytes(byteA, index, number):
    """Encode a unsigned 7 byte integer on a big endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream

    Returns:
        index (int): The incremented index increased by 7
    """
    if number > 72057594037927935:
        number = 72057594037927935
    if number < 0:
        number = 0

    n_byte = number.to_bytes(7, 'big', signed=False)

    pack_into('>7B', byteA, index, n_byte[0], n_byte[1], n_byte[2], n_byte[3], n_byte[4], n_byte[5], n_byte[6])
    index += 7
    return index

def uint56ToLeBytes(byteA, index, number):
    """Encode a unsigned 7 byte integer on a little endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream

    Returns:
        index (int): The incremented index increased by 7
    """
    if number > 72057594037927935:
        number = 72057594037927935
    if number < 0:
        number = 0

    n_byte = number.to_bytes(7, 'little', signed=False)

    pack_into('<7B', byteA, index, n_byte[0], n_byte[1], n_byte[2], n_byte[3], n_byte[4], n_byte[5], n_byte[6])
    index += 7
    return index

def int56ToBeBytes(byteA, index, number):
    """Encode a signed 7 byte integer on a big endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream

    Returns:
        index (int): The incremented index increased by 7
    """
    if number > 36028797018963967:
        number = 36028797018963967
    if number < (-36028797018963968):
        number = (-36028797018963968)

    n_byte = number.to_bytes(7, 'big', signed=True)

    pack_into('>7B', byteA, index, n_byte[0], n_byte[1], n_byte[2], n_byte[3], n_byte[4], n_byte[5], n_byte[6])
    index += 7
    return index

def int56ToLeBytes(byteA, index, number):
    """Encode a signed 7 byte integer on a little endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream

    Returns:
        index (int): The incremented index increased by 7
    """
    if number > 36028797018963967:
        number = 36028797018963967
    if number < (-36028797018963968):
        number = (-36028797018963968)

    n_byte = number.to_bytes(7, 'little', signed=True)

    pack_into('<7B', byteA, index, n_byte[0], n_byte[1], n_byte[2], n_byte[3], n_byte[4], n_byte[5], n_byte[6])
    index += 7
    return index

def uint48ToBeBytes(byteA, index, number):
    """Encode a unsigned 6 byte integer on a big endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream

    Returns:
        index (int): The incremented index increased by 6
    """
    if number > 281474976710655:
        number = 281474976710655
    if number < 0:
        number = 0

    n_byte = number.to_bytes(6, 'big', signed=False)

    pack_into('>6B', byteA, index, n_byte[0], n_byte[1], n_byte[2], n_byte[3], n_byte[4], n_byte[5])
    index += 6
    return index

def uint48ToLeBytes(byteA, index, number):
    """Encode a unsigned 6 byte integer on a little endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream

    Returns:
        index (int): The incremented index increased by 6
    """
    if number > 281474976710655:
        number = 281474976710655
    if number < 0:
        number = 0

    n_byte = number.to_bytes(6, 'little', signed=False)

    pack_into('<6B', byteA, index, n_byte[0], n_byte[1], n_byte[2], n_byte[3], n_byte[4], n_byte[5])
    index += 6
    return index

def int48ToBeBytes(byteA, index, number):
    """Encode a signed 6 byte integer on a big endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream

    Returns:
        index (int): The incremented index increased by 6
    """
    if number > 140737488355327:
        number = 140737488355327
    if number < (-140737488355328):
        number = (-140737488355328)

    n_byte = number.to_bytes(6, 'big', signed=True)

    pack_into('>6B', byteA, index, n_byte[0], n_byte[1], n_byte[2], n_byte[3], n_byte[4], n_byte[5])
    index += 6
    return index

def int48ToLeBytes(byteA, index, number):
    """Encode a signed 6 byte integer on a little endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream

    Returns:
        index (int): The incremented index increased by 6
    """
    if number > 140737488355327:
        number = 140737488355327
    if number < (-140737488355328):
        number = (-140737488355328)

    n_byte = number.to_bytes(6, 'little', signed=True)

    pack_into('<6B', byteA, index, n_byte[0], n_byte[1], n_byte[2], n_byte[3], n_byte[4], n_byte[5])
    index += 6
    return index

def uint40ToBeBytes(byteA, index, number):
    """Encode a unsigned 5 byte integer on a big endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream

    Returns:
        index (int): The incremented index increased by 5
    """
    if number > 1099511627775:
        number = 1099511627775
    if number < 0:
        number = 0

    n_byte = number.to_bytes(5, 'big', signed=False)

    pack_into('>5B', byteA, index, n_byte[0], n_byte[1], n_byte[2], n_byte[3], n_byte[4])
    index += 5
    return index

def uint40ToLeBytes(byteA, index, number):
    """Encode a unsigned 5 byte integer on a little endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream

    Returns:
        index (int): The incremented index increased by 5
    """
    if number > 1099511627775:
        number = 1099511627775
    if number < 0:
        number = 0

    n_byte = number.to_bytes(5, 'little', signed=False)

    pack_into('<5B', byteA, index, n_byte[0], n_byte[1], n_byte[2], n_byte[3], n_byte[4])
    index += 5
    return index

def int40ToBeBytes(byteA, index, number):
    """Encode a signed 5 byte integer on a big endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream

    Returns:
        index (int): The incremented index increased by 5
    """
    if number > 549755813887:
        number = 549755813887
    if number < (-549755813888):
        number = (-549755813888)

    n_byte = number.to_bytes(5, 'big', signed=True)

    pack_into('>5B', byteA, index, n_byte[0], n_byte[1], n_byte[2], n_byte[3], n_byte[4])
    index += 5
    return index

def int40ToLeBytes(byteA, index, number):
    """Encode a signed 5 byte integer on a little endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream

    Returns:
        index (int): The incremented index increased by 5
    """
    if number > 549755813887:
        number = 549755813887
    if number < (-549755813888):
        number = (-549755813888)

    n_byte = number.to_bytes(5, 'little', signed=True)

    pack_into('<5B', byteA, index, n_byte[0], n_byte[1], n_byte[2], n_byte[3], n_byte[4])
    index += 5
    return index


# end supporting 64 bit sizes

def float32ToBeBytes(byteA, index, number):
    """Encode a 4 byte float on a big endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream

    Returns:
        index (int): The incremented index increased by 4
    """
    pack_into('>f', byteA, index, number)
    index += 4
    return index

def float32ToLeBytes(byteA, index, number):
    """Encode a 4 byte float on a little endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream

    Returns:
        index (int): The incremented index increased by 4
    """
    pack_into('<f', byteA, index, number)
    index += 4
    return index

def uint32ToBeBytes(byteA, index, number):
    """Encode a unsigned 4 byte integer on a big endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream

    Returns:
        index (int): The incremented index increased by 4
    """
    if number > 4294967295:
        number = 4294967295
    if number < 0:
        number = 0

    pack_into('>I', byteA, index, number)
    index += 4
    return index

def uint32ToLeBytes(byteA, index, number):
    """Encode a unsigned 4 byte integer on a little endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream

    Returns:
        index (int): The incremented index increased by 4
    """
    if number > 4294967295:
        number = 4294967295
    if number < 0:
        number = 0

    pack_into('<I', byteA, index, number)
    index += 4
    return index

def int32ToBeBytes(byteA, index, number):
    """Encode a signed 4 byte integer on a big endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream

    Returns:
        index (int): The incremented index increased by 4
    """
    if number > 2147483647:
        number = 2147483647
    if number < (-2147483648):
        number = (-2147483648)

    pack_into('>i', byteA, index, number)
    index += 4
    return index

def int32ToLeBytes(byteA, index, number):
    """Encode a signed 4 byte integer on a little endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream

    Returns:
        index (int): The incremented index increased by 4
    """
    if number > 2147483647:
        number = 2147483647
    if number < (-2147483648):
        number = (-2147483648)

    pack_into('<i', byteA, index, number)
    index += 4
    return index

def uint24ToBeBytes(byteA, index, number):
    """Encode a unsigned 3 byte integer on a big endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream

    Returns:
        index (int): The incremented index increased by 3
    """
    if number > 16777215:
        number = 16777215
    if number < 0:
        number = 0

    n_byte = number.to_bytes(3, 'big', signed=False)

    pack_into('>3B', byteA, index, n_byte[0], n_byte[1], n_byte[2])
    index += 3
    return index

def uint24ToLeBytes(byteA, index, number):
    """Encode a unsigned 3 byte integer on a little endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream

    Returns:
        index (int): The incremented index increased by 3
    """
    if number > 16777215:
        number = 16777215
    if number < 0:
        number = 0

    n_byte = number.to_bytes(3, 'little', signed=False)

    pack_into('<3B', byteA, index, n_byte[0], n_byte[1], n_byte[2])
    index += 3
    return index

def int24ToBeBytes(byteA, index, number):
    """Encode a signed 3 byte integer on a big endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream

    Returns:
        index (int): The incremented index increased by 3
    """
    if number > 8388607:
        number = 8388607
    if number < (-8388608):
        number = (-8388608)

    n_byte = number.to_bytes(3, 'big', signed=True)

    pack_into('>3B', byteA, index, n_byte[0], n_byte[1], n_byte[2])
    index += 3
    return index

def int24ToLeBytes(byteA, index, number):
    """Encode a signed 3 byte integer on a little endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream

    Returns:
        index (int): The incremented index increased by 3
    """
    if number > 8388607:
        number = 8388607
    if number < (-8388608):
        number = (-8388608)

    n_byte = number.to_bytes(3, 'little', signed=True)

    pack_into('<3B', byteA, index, n_byte[0], n_byte[1], n_byte[2])
    index += 3
    return index

def uint16ToBeBytes(byteA, index, number):
    """Encode a unsigned 2 byte integer on a big endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream

    Returns:
        index (int): The incremented index increased by 2
    """
    if number > 65535:
        number = 65535
    if number < 0:
        number = 0

    pack_into('>H', byteA, index, number)
    index += 2
    return index

def uint16ToLeBytes(byteA, index, number):
    """Encode a unsigned 2 byte integer on a little endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream

    Returns:
        index (int): The incremented index increased by 2
    """
    if number > 65535:
        number = 65535
    if number < 0:
        number = 0

    pack_into('<H', byteA, index, number)
    index += 2
    return index

def int16ToBeBytes(byteA, index, number):
    """Encode a signed 2 byte integer on a big endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream

    Returns:
        index (int): The incremented index increased by 2
    """
    if number > 32767:
        number = 32767
    if number < (-32768):
        number = (-32768)

    pack_into('>h', byteA, index, number)
    index += 2
    return index

def int16ToLeBytes(byteA, index, number):
    """Encode a signed 2 byte integer on a little endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream

    Returns:
        index (int): The incremented index increased by 2
    """
    if number > 32767:
        number = 32767
    if number < (-32768):
        number = (-32768)

    pack_into('<h', byteA, index, number)
    index += 2
    return index

def float64ToBeBytes(byteA, index, number):
    """Encode a 8 byte float on a big endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream

    Returns:
        index (int): The incremented index increased by 8
    """
    pack_into('>d', byteA, index, number)
    index += 8
    return index

def float64ToLeBytes(byteA, index, number):
    """Encode a 8 byte float on a little endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream

    Returns:
        index (int): The incremented index increased by 8
    """
    pack_into('<d', byteA, index, number)
    index += 8
    return index

def float24ToBeBytes(byteA, index, number, sigbits):
    """Encode a signed 3 byte integer on a big endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream
        sigbits (int): the number of bits to use in the significand of the float.

    Returns:
        index (int): The incremented index increased by 3
    """
    uint24ToBeBytes(float32ToFloat24(number, sigbits), bytes, index)

def float24ToLeBytes(byteA, index, number, sigbits):
    """Encode a signed 3 byte integer on a little endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream
        sigbits (int): the number of bits to use in the significand of the float.

    Returns:
        index (int): The incremented index increased by 3
    """
    uint24ToLeBytes(float32ToFloat24(number, sigbits), bytes, index)

def float16ToBeBytes(byteA, index, number, sigbits):
    """Encode a signed 2 byte integer on a big endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream
        sigbits (int): the number of bits to use in the significand of the float.

    Returns:
        index (int): The incremented index increased by 2
    """
    uint16ToBeBytes(float32ToFloat16(number, sigbits), bytes, index)

def float16ToLeBytes(byteA, index, number, sigbits):
    """Encode a signed 2 byte integer on a little endian byte stream.

    Args:
        number (int): the value to encode
        byteA  (byteArray): The byte stream where the data is encoded
        index  (int): Gives the location of the first byte in the byte stream
        sigbits (int): the number of bits to use in the significand of the float.

    Returns:
        index (int): The incremented index increased by 2
    """
    uint16ToLeBytes(float32ToFloat16(number, sigbits), bytes, index)

# end of fieldencode.py
