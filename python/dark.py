class DarkState:
    def __init__(self):
        self.r = []
        for x in range(8):
            self.r.append(long(0))
        self.j = 0
        self.c = 0

def rotate(a, b,):
    return ((a << b) | (a >> (32 - b)))

def F(state):
    x = 0
    for i in range(8):
        x = state.r[i]
        state.r[i] = (state.r[i] + state.r[(i + 1) & 0x07] + state.j) & 0xFFFFFFFF
        state.r[i] = state.r[i] ^ x
        state.r[i] = rotate(state.r[i], 2) & 0xFFFFFFFF
        state.j = (state.j + state.r[i] + state.c) & 0xFFFFFFFF
        state.c = (state.c + 1) & 0xFFFFFFFF

def keysetup(state, key, nonce):
    m = 0
    for x in range(8):
        state.r[x] = long((ord(key[m]) << 24) + (ord(key[m+1]) << 16) + (ord(key[m+2]) << 8) + ord(key[m+3]))
        m += 4
    m = 0
    n = []
    for x in range(4):
         n.append(long((ord(nonce[m]) << 24) + (ord(nonce[m+1]) << 16) + (ord(nonce[m+2]) << 8) + ord(nonce[m+3])))
         m += 4

    state.r[4] = state.r[4] ^ n[0]
    state.r[5] = state.r[5] ^ n[1]
    state.r[6] = state.r[6] ^ n[2]
    state.r[7] = state.r[7] ^ n[3]

    for i in range(8):
        state.j = (state.j + state.r[i]) & 0xFFFFFFFF
    F(state)

def crypt(msg, key, nonce):
    state = DarkState()
    ctxt = []
    blocks = len(msg) / 4
    extra = len(msg) % 4
    l = 4
    c = 0
    if extra != 0:
        blocks += 1
    keysetup(state, key, nonce)
    for b in range(blocks):
        F(state)
        output = (((((((state.r[0] + state.r[6]) ^ state.r[1]) + state.r[5]) ^ state.r[2]) + state.r[4]) ^ state.r[3]) + state.r[7]) & 0xFFFFFFFF
        k = [0] * 4
        k[0] = (output & 0x000000FF)
        k[1] = (output & 0x0000FF00) >> 8
        k[2] = (output & 0x00FF0000) >> 16
        k[3] = (output & 0xFF000000) >> 24
        if b == (blocks -1) and extra != 0:
            l = extra
        for i in range(l):
            o = ord(msg[c]) ^ k[i]
            c += 1
            ctxt.append(chr(o))
    return ctxt
