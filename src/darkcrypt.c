#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "dyefamily.c"
#include "ganja.c"

int keylen = 32;
struct dark_state {
    uint32_t r[8];
    uint32_t j;
    uint32_t c;
};

uint32_t rotate(uint32_t a, uint32_t b) {
    return ((a << b) | (a >> (32 - b)));
}

void *F(struct dark_state *state) {
    int i;
    uint32_t x;
    for (i = 0; i < 8; i++) {
<<<<<<< HEAD
        x = state->r[i];
        state->r[i] = (state->r[i] + state->r[(i + 1) % 8] + state->j) & 0xFFFFFFFF;
        state->r[i] = state->r[i] ^ x;
        state->r[i] = rotate(state->r[i], 2) & 0xFFFFFFFF;
        state->j = (state->j + state->r[i] + state->c) & 0xFFFFFFFF;
        state->c = state->c + 1 & 0xFFFFFFFF;
=======
        x = r[i];
        r[i] = (r[i] + r[(i + 1) & 0x07] + j) & 0xFFFFFFFF;
        r[i] = r[i] ^ x;
        r[i] = rotate(r[i], 2) & 0xFFFFFFFF;
        j = (j + r[i] + ct) & 0xFFFFFFFF;
        ct = ct + 1 & 0xFFFFFFFF;
>>>>>>> 49b4cba365c180ffcd64042317baf4874b581e51
    }
}

void *keysetup(struct dark_state *state, unsigned char *key, unsigned char *nonce) {
    uint32_t n[4];
    state->r[0] = (key[0] << 24) + (key[1] << 16) + (key[2] << 8) + key[3];
    state->r[1] = (key[4] << 24) + (key[5] << 16) + (key[6] << 8) + key[7];
    state->r[2] = (key[8] << 24) + (key[9] << 16) + (key[10] << 8) + key[11];
    state->r[3] = (key[12] << 24) + (key[13] << 16) + (key[14] << 8) + key[15];
    state->r[4] = (key[16] << 24) + (key[17] << 16) + (key[18] << 8) + key[19];
    state->r[5] = (key[20] << 24) + (key[21] << 16) + (key[22] << 8) + key[23];
    state->r[6] = (key[24] << 24) + (key[25] << 16) + (key[26] << 8) + key[27];
    state->r[7] = (key[28] << 24) + (key[29] << 16) + (key[30] << 8) + key[31];

    n[0] = (nonce[0] << 24) + (nonce[1] << 16) + (nonce[2] << 8) + nonce[3];
    n[1] = (nonce[4] << 24) + (nonce[5] << 16) + (nonce[6] << 8) + nonce[7];
    n[2] = (nonce[8] << 24) + (nonce[9] << 16) + (nonce[10] << 8) + nonce[11];
    n[3] = (nonce[12] << 24) + (nonce[13] << 16) + (nonce[14] << 8) + nonce[15];

    state->r[4] = state->r[4] ^ n[0];
    state->r[5] = state->r[5] ^ n[1];
    state->r[6] = state->r[6] ^ n[2];
    state->r[7] = state->r[7] ^ n[3];

    state->j = 0;
    state->c = 0;
    for (int i = 0; i < 8; i++) {
        state->j = (state->j +  state->r[i]) & 0xFFFFFFFF;
    }
    F(state);
}

void usage() {
    printf("dark <encrypt/decrypt> <input file> <output file> <password>\n");
    exit(0);
}

int main(int argc, char *argv[]) {
    FILE *infile, *outfile, *randfile;
    char *in, *out, *mode;
    unsigned char *data = NULL;
    unsigned char *buf = NULL;
    int x = 0;
    int i = 0;
    int ch;
    int buflen = 131072;
    int bsize;
    uint32_t output;
    struct dark_state state;
    unsigned char *key[keylen];
    unsigned char *password;
    int k[4] = {0};
    int nonce_length = 16;
    int iterations = 10;
    unsigned char *salt = "MyDarkCipher";
    unsigned char nonce[nonce_length];
    unsigned char block[buflen];
    if (argc != 5) {
        usage();
    }
    mode = argv[1];
    in = argv[2];
    out = argv[3];
    password = argv[4];
    infile = fopen(in, "rb");
    fseek(infile, 0, SEEK_END);
    long fsize = ftell(infile);
    fseek(infile, 0, SEEK_SET);
    outfile = fopen(out, "wb");
    int c = 0;
    if (strcmp(mode, "encrypt") == 0) {
        long blocks = fsize / buflen;
        long extra = fsize % buflen;
        if (extra != 0) {
            blocks += 1;
        }
        wrzeszcz_random(nonce, nonce_length);
        fwrite(nonce, 1, nonce_length, outfile);
        ganja_kdf(password, strlen(password), key, iterations, keylen, salt);
        keysetup(&state, key, nonce);
        for (int d = 0; d < blocks; d++) {
            fread(block, buflen, 1, infile);
            bsize = sizeof(block);
	    c = 0;
            for (int b = 0; b < (bsize / 4); b++) {
		F(&state);
		output = (((((((state.r[0] + state.r[6]) ^ state.r[1]) + state.r[5]) ^ state.r[2]) + state.r[4]) ^ state.r[3]) + state.r[7]) & 0xFFFFFFFF;
		k[0] = (output & 0x000000FF);
		k[1] = (output & 0x0000FF00) >> 8;
		k[2] = (output & 0x00FF0000) >> 16;
		k[3] = (output & 0xFF000000) >> 24;
		for (c = (b * 4); c < ((b *4) + 4); c++) {
                    block[c] = block[c] ^ k[c % 4];
		}
            }
            if (d == (blocks - 1) && extra != 0) {
                bsize = extra;
            }
            fwrite(block, 1, bsize, outfile);
        }
    }
    else if (strcmp(mode, "decrypt") == 0) {
        long blocks = (fsize - nonce_length) / buflen;
        long extra = (fsize - nonce_length) % buflen;
        if (extra != 0) {
            blocks += 1;
        }
        fread(nonce, 1, nonce_length, infile);
        ganja_kdf(password, strlen(password), key, iterations, keylen, salt);
        keysetup(&state, key, nonce);
        for (int d = 0; d < blocks; d++) {
            fread(block, buflen, 1, infile);
            bsize = sizeof(block);
            for (int b = 0; b < (bsize / 4); b++) {
		F(&state);
		output = (((((((state.r[0] + state.r[6]) ^ state.r[1]) + state.r[5]) ^ state.r[2]) + state.r[4]) ^ state.r[3]) + state.r[7]) & 0xFFFFFFFF;
		k[0] = (output & 0x000000FF);
		k[1] = (output & 0x0000FF00) >> 8;
		k[2] = (output & 0x00FF0000) >> 16;
		k[3] = (output & 0xFF000000) >> 24;
		for (c = (b * 4); c < ((b *4) + 4); c++) {
                    block[c] = block[c] ^ k[c % 4];
		}
            }
            if ((d == (blocks - 1)) && extra != 0) {
                bsize = extra;
            }
            fwrite(block, 1, bsize, outfile);
        }
    }
    fclose(infile);
    fclose(outfile);
    return 0;
}
