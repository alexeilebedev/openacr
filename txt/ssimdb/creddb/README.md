## creddb - Credentials a person or service holds: tokens, ssh keys, and the master key that seals them
<a href="#creddb"></a>
The tables [credd](/txt/exe/credd/README.md) keeps under `~/.ssh/credd`.
`creddb.kind` and `creddb.perm` are the provider vocabulary, compiled into the
binary from `data/creddb`: which permissions each kind of token offers, the
page where a token of that kind is requested, and the steps a person follows to
get one.  `creddb.cred`, `creddb.credperm` and `creddb.master` are the store
itself, and the copies under `data/creddb` are empty.  A cred is a token, an
ssh key or a link, told apart by its kind.  A secret field holds base64 of
a nonce, the ciphertext and the tag, sealed with AES-256-GCM under a key scrypt
derives from the master password and the master record's salt.
