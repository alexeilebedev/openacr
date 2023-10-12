## Writing Readmes

The top-level `README.md` file is created by processing 
specific .md files under txt/.
These files are described in the readme table (`acr readme`).
Readme files can be in-line or included via a link. This is configured by the `inl` flag:

```
inline-command: acr readme | head -1
dev.readme  gitfile:txt/about.md                   inl:Y  sandbox:N  filter:""               comment:""
```

The top-level README.md file is created with `make readme` or, in reality, `atf_ci readme`.
It is a by-product of normalization and has to be up-to-date for every commit.

In addition, output of arbitrary commands can be embedded inside readme files using
the inline-command directive. Everything after this directive and before the next triple-backtick
is replaced with the output of specified command. An example of such a command can be seen above

