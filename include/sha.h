// Copyright (C) 2025-2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2023 Astra
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2008-2012 AlgoEngineering LLC
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Header: include/sha.h
//
// ----------------------------------------------------------------------------
// openssl crypto api to compute SHA1 digest step-by-step
// Sha1 steb-by-step computation context
// SHA_CONTEXT   context to use in SHA1_* functions
// SHA_DIGEST    computed 20-bytes SHA1 digest
// INIT_FLAG     true if context has been initialized
// FINAL_FLAG    true if context has been finalized (so it is possible to read SHA_DIGEST)
// State machine:
// Init -> Update ... Update -> Finish -> (GetDigest), Init -> ...

#pragma once
#include <openssl/evp.h>

struct Sha1Ctx {
    EVP_MD_CTX* sha_context;
    u8          sha_digest[EVP_MAX_MD_SIZE];
    algo::Bool        final_flag;
    Sha1Ctx();
    ~Sha1Ctx();
};

// Initialize Sha1 context
inline Sha1Ctx::Sha1Ctx() {
    sha_context = EVP_MD_CTX_new();
    vrfy(sha_context, "EVP_MD_CTX_new");
    vrfy(EVP_DigestInit(sha_context, EVP_get_digestbyname("sha1")),"SHA1_Init");
    memset(&sha_digest,0,sizeof(sha_digest));
}

inline Sha1Ctx::~Sha1Ctx() {
    EVP_MD_CTX_free(sha_context);
}

// Update Sha1 context with new data
inline void Update(Sha1Ctx &ctx, algo::memptr data) {
    vrfy(!ctx.final_flag, "SHA context has already been finalized");
    vrfy(EVP_DigestUpdate(ctx.sha_context, data.elems, data.n_elems), "SHA1_Update");
}

// Finalize Sha1 context, and compute the digest
inline void Finish(Sha1Ctx &ctx) {
    vrfy(!ctx.final_flag, "SHA context has already been finalized");
    vrfy(EVP_DigestFinal(ctx.sha_context,ctx.sha_digest,NULL), "SHA1_Final");
    ctx.final_flag.value = true;
}

// Get the digest
inline algo::Signature GetDigest(Sha1Ctx &ctx) {
    algo::Signature ret;
    vrfy(ctx.final_flag,"SHA context has not been finalized");
    memcpy(ret.signature_elems, ctx.sha_digest, sizeof(ret.signature_elems));
    return ret;
}
