// (C) AlgoEngineering LLC 2008-2012
// (C) 2013-2019 NYSE | Intercontinental Exchange
//
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
//
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Header: include/sha.h
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
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

struct Sha1Ctx {
    SHA_CTX sha_context;
    u8      sha_digest[SHA_DIGEST_LENGTH];
    Bool    final_flag;
    Sha1Ctx();
};

// Initialize Sha1 context
inline Sha1Ctx::Sha1Ctx() {
    vrfy(SHA1_Init(&sha_context),"SHA1_Init");
    memset(&sha_digest,0,sizeof(sha_digest));
}

// Update Sha1 context with new data
inline void Update(Sha1Ctx &ctx, algo::memptr data) {
    vrfy(!ctx.final_flag, "SHA context has already been finalized");
    vrfy(SHA1_Update(&ctx.sha_context, data.elems, data.n_elems), "SHA1_Update");
}

// Finalize Sha1 context, and compute the digest
inline void Finish(Sha1Ctx &ctx) {
    vrfy(!ctx.final_flag, "SHA context has already been finalized");
    vrfy(SHA1_Final(&ctx.sha_digest[0], &ctx.sha_context), "SHA1_Final");
    ctx.final_flag.value = true;
}

// Get the digest
inline algo::Sha1sig GetDigest(Sha1Ctx &ctx) {
    algo::Sha1sig ret;
    vrfy(ctx.final_flag,"SHA context has not been finalized");
    memcpy(ret.sha1sig_elems, ctx.sha_digest, sizeof(ret.sha1sig_elems));
    return ret;
}
