# DarkCipher

DarkCipher is a 256 bit stream cipher aimed at providing no vector of attack other than brute force.  Dark maintains a 256 bit state and outputs 32 bits from a complex output function leaving no room for clues regarding the state.  Dark is fast and formidable.  Dark takes a 256 bit key and 128 bit nonce.

In order to break DarkCipher, one must discover the entire 256 bit state.  DarkCipher also includes the state from the previous round in it's calculations.  Best attack vector is brute force.
