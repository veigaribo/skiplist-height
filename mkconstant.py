#!/usr/bin/env python3

import math
import sys
import io
from typing import NoReturn

INT_SIZE_BITS = 32
RANGE_BITS = 6
SCALE_BITS = INT_SIZE_BITS - RANGE_BITS

help = f"""
Given a probability `p` in the range (0, 1), returns its log base 2 in fixed point
format, with {RANGE_BITS} bits for the integer part and {SCALE_BITS} bits for the
fractional part.

Used in the function `get_random_value` to have the effect of changing its `p` to
the given value.

Example:
$ ./mkconstant.py 0.3125
integral 1.0 mantissa 0.6780719051126376
0b00000110101101100101100001111011
"""


def error(msg: str) -> NoReturn:
    print(msg, file=sys.stderr)
    exit(1)


if __name__ == '__main__':
    if len(sys.argv) == 1:
        print(help)
        exit(0)

    if len(sys.argv) != 2:
        error('expected a numeric literal as the *one and only argument*', )

    arg = sys.argv[1]
    try:
        n = float(arg)
    except ValueError as e:
        error('expected a *numeric literal* as the one and only argument', )

    if not (0 < n < 1):
        error('argument should be between 0 and 1 exclusive')

    log2_n = -1.0 / math.log2(n)

    mantissa, integral = math.modf(log2_n)
    integral_bin = format(int(integral), f'0{RANGE_BITS}b')

    print('integral', integral, 'mantissa', mantissa, file=sys.stderr)

    if len(integral_bin) > RANGE_BITS:
        error(f'whole part of number can\'t fit in {RANGE_BITS} bits')

    writer = io.StringIO()
    bin_len = 0

    _ = writer.write('0b')
    bin_len += writer.write(integral_bin)

    remaining_bits = INT_SIZE_BITS - bin_len

    for i in range(remaining_bits):
        mantissa = mantissa * 2

        if mantissa > 1:
            mantissa = mantissa - 1
            bin_len += writer.write('1')
        else:
            bin_len += writer.write('0')

    print(writer.getvalue())
