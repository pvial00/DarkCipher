# DarkCipher

DarkCipher is a 256 bit stream cipher aimed at providing no vector of attack other than brute force.  Dark maintains a 256 bit state and outputs 32 bits from a complex output function leaving no room for clues regarding the state.  Dark is fast and formidable.  Dark takes a 256 bit key and 128 bit nonce.

In order to break DarkCipher, one must discover the entire 256 bit state.  DarkCipher also includes the state from the previous round in it's calculations.  Best attack vector is brute force.

# Algorithm description

Dark initializes a 256 bit array (8 x 32 bit words) with the key.  Half of the array is XORed with the nonce and first round begin.  Each round, each array elemebt is added modulo 2^32 to the element to the right of it, XORed with the value prior to the addition and then rotated left 2 bits.  The 8, 32 bit words are then added and XORed together to create 32 output bits which is XORed with the 4 input bytes.

The complexity to solve for the state at any given block of 32 bits in the ciphertext is the same complexity to brute force the key.
