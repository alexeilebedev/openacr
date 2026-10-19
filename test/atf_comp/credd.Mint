# setenv HOME:$HOMEDIR
# setenv CREDD_PASSWORD:pw1
# start credd cmd:"$bindir/credd -mint:bot -in:$tempdir/data -server:https://gitlab.example.com -account:algornd -src:@$tempdir/tok"
# eof credd
credd -> Minting bot, a gitlab_group token acting as developer with api,read_repository, for algornd at https://gitlab.example.com.
credd -> credd.newstore  dir:temp/atf_comp/credd.Mint/home/.ssh/credd  comment:"no master password yet; choose one"
credd -> credd.master  dir:temp/atf_comp/credd.Mint/home/.ssh/credd  comment:"master password set"
credd -> credd.nominter  server:https://gitlab.example.com  comment:"no gitlab token of yours for this server is in the store to ask with; the steps below are for a person"
credd -> Only an Owner of the group algornd can make this token. Send an Owner this request, or do it yourself if you are one: open https://gitlab.example.com/groups/algornd/-/settings/access_tokens and choose Add new token, with the name bot, the role developer, the scopes api,read_repository, and the latest expiration date the form allows. Then Create group access token. Copy the token; GitLab shows it once. Paste it below. Revoke the old token on the same page once the new one works.
credd -> credd.add  cred:bot  kind:gitlab_group  linkto:""  perm:api,read_repository
# exit credd code:0
# start credd-2 cmd:"$bindir/credd -mint:pat -in:$tempdir/data -server:https://gitlab.example.com -src:@$tempdir/tok"
# eof credd-2
credd-2 -> Minting pat, a gitlab token with read_api at https://gitlab.example.com.
credd-2 -> Sign in to https://gitlab.example.com as and open https://gitlab.example.com/-/user_settings/personal_access_tokens?name=pat&scopes=read_api, which is the access-tokens page with the name pat and the scopes read_api filled in. Set an expiration, then Create personal access token. Copy the token; GitLab shows it once. Paste it below. Revoke the old token on the same page once the new one works.
credd-2 -> credd.add  cred:pat  kind:gitlab  linkto:""  perm:read_api
# exit credd-2 code:0
# start credd-3 cmd:"$bindir/credd -mint:bot -in:$tempdir/data -server:https://gitlab.example.com -account:algornd -src:@$tempdir/tok"
# eof credd-3
credd-3 -> credd.error  credd.exists  name:bot  comment:"a credential of this name is in the store; -remove it first"
# exit credd-3 code:1
# start credd-4 cmd:"$bindir/credd -mint:ghost -in:$tempdir/data -src:@$tempdir/tok"
# eof credd-4
credd-4 -> credd.error  credd.noneed  name:ghost  in:temp/atf_comp/credd.Mint/data  comment:"dev.cred has no row of this name; a tool's credential is declared there, with its role in dev.credrole and permissions in dev.credperm"
# exit credd-4 code:1
# start credd-5 cmd:"$bindir/credd -mint:norole -in:$tempdir/data -server:https://gitlab.example.com -account:algornd -src:@$tempdir/tok"
# eof credd-5
credd-5 -> credd.error  credd.norole  name:norole  kind:gitlab_group  comment:"a token of this kind acts in a role; dev.credrole has no row for this credential"
# exit credd-5 code:1
# start credd-6 cmd:"$bindir/credd -mint:acct -in:$tempdir/data -server:https://gitlab.example.com -src:@$tempdir/tok"
# eof credd-6
credd-6 -> credd.error  credd.badopt  mint:acct  comment:"-server and -account say where the token is made: the GitLab URL and the group or project path"
# exit credd-6 code:1
# start credd-7 cmd:"$bindir/credd -list -format:ssim"
# eof credd-7
credd-7 -> creddb.cred  cred:bot  kind:gitlab_group  linkto:""  server:https://gitlab.example.com  account:algornd  pubkey:***  created:***  expires:""  secret:***  comment:"the review bot"
credd-7 ->   creddb.credperm  credperm:bot/api  comment:""
credd-7 ->   creddb.credperm  credperm:bot/read_repository  comment:""
credd-7 -> creddb.cred  cred:pat  kind:gitlab  linkto:""  server:https://gitlab.example.com  account:""  pubkey:***  created:***  expires:""  secret:***  comment:"a person reading the api"
credd-7 ->   creddb.credperm  credperm:pat/read_api  comment:""
# exit credd-7 code:0
# start credd-8 cmd:"$bindir/credd -renew:bot -in:$tempdir/data -src:@$tempdir/tok"
# eof credd-8
credd-8 -> Renewing bot, a gitlab_group token for algornd at https://gitlab.example.com.
credd-8 -> It was issued with these permissions, which the new token needs too:
credd-8 ->   api: Full read and write access to the API for the group's projects, as far as the role allows
credd-8 ->   read_repository: Read repositories over git and through the API
credd-8 -> Only an Owner of the group algornd can make this token. Send an Owner this request, or do it yourself if you are one: open https://gitlab.example.com/groups/algornd/-/settings/access_tokens and choose Add new token, with the name bot, the role developer, the scopes api,read_repository, and the latest expiration date the form allows. Then Create group access token. Copy the token; GitLab shows it once. Paste it below. Revoke the old token on the same page once the new one works.
credd-8 -> The new value replaces the old one in the store once you paste it and give the master password; the running daemon serves it at once.
credd-8 -> credd.renewed  cred:bot  expires:""
# exit credd-8 code:0
# clearenv
