## lib_cred - Client library: fetch a credential from credd by name
<a href="#lib_cred"></a>
A program that needs a token calls `lib_cred::Fetch` with its own name and the
token's symbolic name.  The call connects to the daemon of the default store,
`$HOME/.ssh/credd/credd.sock`, asks, and returns the value, or false with a
message saying why: no daemon answers, or the daemon refused the name.  The
call blocks for at most two seconds.  [credd](/txt/exe/credd/README.md) is the
daemon.

The protocol is one ssim tuple each way on a unix socket, and the tuples are
this library's ctypes: `lib_cred.ShowReq` asks for a value and gets
`lib_cred.Value` or `lib_cred.Error`; `lib_cred.StatusReq` gets
`lib_cred.Status`; `lib_cred.ReloadReq` and `lib_cred.StopReq` get
`lib_cred.Done`.  `lib_cred::Request` sends any of them to a socket and returns
the reply line, which is what credd's own command line uses.

A tool that also runs where nobody starts a daemon, on a node or a ci runner,
calls `lib_cred::FetchFile` with the credential's name and the token file it
stands for.  It gets credd's secret when a daemon holds the name, and the
file's content otherwise, so the credential and the file hold the same text.
`lib_cred::EvalVar` reads one variable out of that text when it is the lines a
shell sources.  `lib_cred::SysEvalStdin` runs a command with a secret on its
standard input, which is how a tool hands a token to curl without putting it on
a command line.

| tool | credential | file |
|---|---|---|
| gli | the token of a gitlab server, named in `~/.ssh/gli.ssim` | `~/.ssh/gli.ssim` |
| a cloudflare client | a cloudflare token, named by its account or the domain | `~/.ssh/<name>_token` |
| a hardware vendor client | a vendor's api key, named by the account | `~/.ssh/<name>_token` |
| an AWS client | an IAM user's access key, named by the user | `~/.ssh/<user>.token` |
| gptcli | `openai` | `~/.ssh/openai_api_secrets` |
