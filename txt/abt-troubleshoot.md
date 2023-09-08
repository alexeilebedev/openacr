## Troubleshooting build errors

- check compatibility with OS distribution and compiler;
- install prerequisites;
- check that PATH contains 'bin' directory;
- force abt rebuild with 'ai -clean';
- clean caches: `ccache-clean`, `gcache-clean';
- if nothing helps, clean everything with 'git clean -xdf'.
