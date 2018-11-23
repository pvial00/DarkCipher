# DarkCipher

DarkCipher is a 256 bit stream cipher aimed at providing no vector of attack other than brute force.  Dark maintains a 256 bit state and outputs 32 bits from a complex output function leaving no room for clues regarding the state.  Dark is fast and formidable.  Dark takes a 256 bit key and 128 bit nonce.

In order to break DarkCipher, one must discover the entire 256 bit state.  DarkCipher also includes the state from the previous round in it's calculations.  Best attack vector is brute force.

# Algorithm description

Dark initializes a 256 bit array or register (8 x 32 bit words) with the key 4 bytes per word.  The right half of the array is XORed with the nonce.  J, (a pseudo random accumulator) is calculated by suming itself and the sum of all registers. The the round function is applied once in preperation for the first round.
  
For each round, the round function is applied.  First each register word is added modulo 2^32 to the word to the right of it. The result is XORed with the value prior to the addition and then rotated left 2 bits.

The round function can best be described by this C code:

void F(uint32_t j, uint32_t ct) {

    int i;

    uint32_t x;

    for (i = 0; i < 8; i++) {

        x = r[i];

        r[i] = (r[i] + r[(i + 1) % 8] + j) & 0xFFFFFFFF;

        r[i] = r[i] ^ x;

        r[i] = rotateleft(r[i], 2) & 0xFFFFFFFF;

        j = (j + r[i]) & 0xFFFFFFFF;


    }

}

The register then outputs 32 bits using the following nested function:

(((((((r[0] + r[6]) ^ r[1]) + r[5]) ^ r[2]) + r[4]) ^ r[3]) + r[7]) & 0xFFFFFFFF;

Lastly, 4 bytes are unpacked from the 32 bit word and XORed with the first 4 plaintext bytes.  This cycle continues until the plaintext input stream is exhausted.

The complexity to solve for the state at any given block of 32 bits in the ciphertext is the same complexity to brute force the key (2^256 complexity).  The complexity to calculate the state or to compute the next state at any given point in time is also 2^256.

The complexity to solve for the state at any given block of 32 bits in the ciphertext is the same complexity to brute force the key.
