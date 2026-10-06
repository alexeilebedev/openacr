// Copyright (C) 2026 AlgoX2 Corp
//
// License: Apache
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Target: credd (exe) -- Hold a session's credentials unlocked in memory and hand them to the tools that authenticate
// Exceptions: yes
// Source: cpp/credd/seal.cpp
//
// The cryptography, and nothing else: a key derived from the master password
// with scrypt, and a secret sealed under that key with AES-256-GCM.  Every
// sealed value in the store has one shape, base64 of nonce, ciphertext and
// tag, so the same two functions open a token, a private key and the master
// record's verifier.

#include "include/algo.h"
#include "include/credd.h"
#include <openssl/evp.h>
#include <openssl/rand.h>

// Fill OUT with N bytes from the system random source.  False when the source
// fails, which leaves OUT empty.
bool credd::RandomBytes(int n, algo::cstring &out) {
    out = "";
    algo::aryptr<char> buf = ch_AllocN(out, n);
    bool ok = RAND_bytes((unsigned char*)buf.elems, n) == 1;
    if (!ok) {
        out = "";
    }
    return ok;
}

// Derive the 32-byte master key from PASSWORD and SALT with scrypt, at a cost
// of N=2^15, r=8, p=1, which takes a fraction of a second and 32MB.  False
// when libcrypto refuses, and OUT_KEY is then empty.
bool credd::DeriveKey(algo::strptr password, algo::strptr salt, algo::cstring &out_key) {
    out_key = "";
    algo::aryptr<char> key = ch_AllocN(out_key, 32);
    bool ok = EVP_PBE_scrypt(password.elems, password.n_elems
                             , (const unsigned char*)salt.elems, salt.n_elems
                             , 32*1024, 8, 1, 64*1024*1024
                             , (unsigned char*)key.elems, 32) == 1;
    if (!ok) {
        out_key = "";
    }
    return ok;
}

// Seal PLAIN under the 32-byte KEY: a fresh 12-byte nonce, the ciphertext,
// and the 16-byte tag, concatenated and written to OUT as base64.  False when
// KEY has the wrong length or libcrypto fails.
bool credd::Seal(algo::strptr key, algo::strptr plain, algo::cstring &out) {
    out = "";
    algo::cstring nonce;
    bool ok = elems_N(key) == 32 && credd::RandomBytes(12, nonce);
    EVP_CIPHER_CTX *ctx = ok ? EVP_CIPHER_CTX_new() : NULL;
    ok = ok && ctx != NULL;
    ok = ok && EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL
                                  , (const unsigned char*)key.elems
                                  , (const unsigned char*)nonce.ch_elems) == 1;
    algo::cstring raw;
    raw << nonce;
    int nout = 0;
    if (ok) {
        algo::aryptr<char> ct = ch_AllocN(raw, elems_N(plain));
        ok = EVP_EncryptUpdate(ctx, (unsigned char*)ct.elems, &nout
                               , (const unsigned char*)plain.elems, elems_N(plain)) == 1
            && nout == elems_N(plain);
    }
    unsigned char tail[16];
    ok = ok && EVP_EncryptFinal_ex(ctx, tail, &nout) == 1 && nout == 0;
    ok = ok && EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tail) == 1;
    if (ctx) {
        EVP_CIPHER_CTX_free(ctx);
    }
    if (ok) {
        raw << algo::strptr((char*)tail, 16);
        algo::strptr_PrintBase64(raw, out);
    }
    return ok;
}

// Open SEALED, a value Seal produced, under KEY into OUT.  False when the
// base64 does not decode, the value is too short to carry a nonce and a tag,
// KEY has the wrong length, or the tag does not verify, which is what a wrong
// key or a damaged record looks like.  OUT is empty on failure.
bool credd::Unseal(algo::strptr key, algo::strptr sealed, algo::cstring &out) {
    out = "";
    algo::cstring raw;
    bool ok = elems_N(key) == 32 && algo::strptr_ReadBase64(sealed, raw) && raw.ch_n >= 28;
    EVP_CIPHER_CTX *ctx = ok ? EVP_CIPHER_CTX_new() : NULL;
    ok = ok && ctx != NULL;
    ok = ok && EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL
                                  , (const unsigned char*)key.elems
                                  , (const unsigned char*)raw.ch_elems) == 1;
    int nct = ok ? raw.ch_n - 28 : 0;
    int nout = 0;
    if (ok) {
        algo::aryptr<char> plain = ch_AllocN(out, nct);
        ok = EVP_DecryptUpdate(ctx, (unsigned char*)plain.elems, &nout
                               , (const unsigned char*)raw.ch_elems + 12, nct) == 1
            && nout == nct;
    }
    ok = ok && EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, raw.ch_elems + 12 + nct) == 1;
    unsigned char tail[16];
    ok = ok && EVP_DecryptFinal_ex(ctx, tail, &nout) == 1;
    if (ctx) {
        EVP_CIPHER_CTX_free(ctx);
    }
    if (!ok) {
        out = "";
    }
    return ok;
}
