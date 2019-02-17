package main

import  (
    "os"
    "io/ioutil"
    "crypto/rand"
    "crypto/sha256"
    "strings"
)

type dark_state struct {
    r[8] uint32
    j uint32
    c uint32
}

func rotate(a uint32, b uint32) (uint32) {
    return ((a << b) | (a >> (32 - b)))
}

func F(state *dark_state) {
    var x uint32
    for i := 0; i < 8; i++ {
        x = state.r[i]
        state.r[i] = (state.r[i] + state.r[(i + 1) & 0x07] + state.j) & 0xFFFFFFFF
        state.r[i] = state.r[i] ^ x
        state.r[i] = rotate(state.r[i], 2) & 0xFFFFFFFF
        state.j = (state.j + state.r[i] + state.c) & 0xFFFFFFFF
        state.c = (state.c + 1) & 0xFFFFFFFF
    }
}

func keysetup(state *dark_state, key [32]byte, nonce []byte) {
    n := new([4]uint32)
    keylen := len(key)
    c := 0
    for i := 0; i < (keylen / 4); i++ {
        state.r[i] = uint32((key[c] << 24) + (key[c+1] << 16) + (key[c+2] << 8) + key[c+3])
        c = c + 4
    }
    c = 0
    for i := 0; i < (16 / 4); i++ {
        n[i] = uint32((nonce[c] << 24) + (nonce[c+1] << 16) + (nonce[c+2] << 8) + nonce[c+3])
        c = c + 4
    }
    state.r[4] = state.r[4] ^ n[0]
    state.r[5] = state.r[5] ^ n[1]
    state.r[6] = state.r[6] ^ n[2]
    state.r[7] = state.r[7] ^ n[3]
 
    state.j = 0
    for i := 0; i < 8; i++ {
        state.j = (state.j + state.r[i]) & 0xFFFFFFFF
    }
    F(state)
}

func getoutput(state *dark_state) (uint32) {
    return uint32(state.r[0] ^ state.r[1] ^ state.r[2] ^ state.r[3] ^ state.r[4] ^ state.r[5] ^ state.r[6] ^ state.r[7])
}

func dark_crypt(msg []byte, key [32]byte, nonce []byte) {
    state := dark_state{}
    msglen := len(msg)
    c := 0
    l := 4
    o := 0
    k := new([4]uint8)
    blocks := msglen / 4
    extra := msglen % 4
    if extra != 0 {
        blocks = blocks + 1
    }
    keysetup(&state, key, nonce)
    for i := 0; i < (msglen / 4); i++ {
        F(&state)
        output := getoutput(&state)
        k[0] = uint8((output & 0x000000FF))
        k[1] = uint8((output & 0x0000FF00)  >> 8)
        k[2] = uint8((output & 0x00FF0000) >> 16)
        k[3] = uint8((output & 0xFF000000) >> 24)
        if (i == (blocks - 1) && (extra != 0)) {
            l = extra
        }
        for o = 0; o < l; o++ {
            msg[c] = byte(uint8(msg[c]) ^ k[o])
            c = c + 1
        }
    }
}

func check(e error) {
    if e != nil {
        panic(e)
    }
}

func readfile_data(filename string) ([]byte) {
    data, err := ioutil.ReadFile(filename)
    check(err)
    return data
}

func writefile_data(data []byte, filename string) {
    err := ioutil.WriteFile(filename, data, 0644)
    check(err)
}

func getrandom(num_bytes int) ([]byte) {
    mybytes := make([]byte, num_bytes)
    rand.Read(mybytes)
    return mybytes
}

func encrypt(msg []byte, key [32]byte) ([]byte) {
    nonce := getrandom(16)
    dark_crypt(msg, key, nonce)
    msgpack := make([]byte, 0)
    msgpack = append(msgpack, nonce...)
    msgpack = append(msgpack, msg...)
    return msgpack
}

func decrypt(msgpack []byte, key [32]byte) ([]byte) {
    nonce := msgpack[0:16]
    msg := msgpack[16:]
    dark_crypt(msg, key, nonce)
    return msg
}

func main() {
    mode := os.Args[1]
    infile := os.Args[2]
    outfile := os.Args[3]
    password := os.Args[4]
    key := sha256.Sum256([]byte(password))
    if strings.Compare(mode, "e") == 0 {
        msg := readfile_data(infile)
        msgpack := encrypt(msg, key)
        writefile_data(msgpack, outfile)
    } else if strings.Compare(mode, "d") == 0 {
        msgpack := readfile_data(infile)
        msg := decrypt(msgpack, key)
        writefile_data(msg, outfile)
    }
}
