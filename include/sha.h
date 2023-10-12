// Copyright (C) 2008-2012 AlgoEngineering LLC
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2020-2023 Astra
// Copyright (C) 2023 AlgoRND
//
// License: GPL
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
    Bool        final_flag;
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
inline algo::Sha1sig GetDigest(Sha1Ctx &ctx) {
    algo::Sha1sig ret;
    vrfy(ctx.final_flag,"SHA context has not been finalized");
    memcpy(ret.sha1sig_elems, ctx.sha_digest, sizeof(ret.sha1sig_elems));
    return ret;
}
