## OpenACR
<a href="#openacr"></a>

This is OpenACR, a meta-programming language for generative systems programming.
It consists of a powerful and extensible set of tools for creating programs and meta-algorithms,
and writing code as data. OpenACR is published at [https://github.com/alexeilebedev/openacr](https://github.com/alexeilebedev/openacr)

It is the result of over 15 years of development and
production use. OpenACR was used to build mission-critical financial systems that handle hundreds of billions
of messages daily at microsecond latencies, financial exchanges, distributed HFT platforms, CI systems, deployment systems, alerts
and monitoring systems. It was also used to build itself -- 95% of all the source code here is generated
by OpenACR's code generator (amc) from ascii relational tables.

There are two key concepts in OpenACR: [Ssimfiles](ssim.md) and [C++ code generation](/txt/exe/amc/README.md).

The ideal use case is realized when it is taken as a core of a project, forming an ecosystem of commands and corresponding
configuration data files, around which the project is grown an co-evolved with the schema.
It can also be used if your project doesn't use C++ (in this case, focus on ssimfiles and [acr](/txt/exe/acr/README.md).

### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
<!-- dev.mdmark  mdmark:TOC  state:BEG_AUTO  param:Toc -->
&#128196; [Setup And Installation](/txt/setup.md)<br/>
&#128196; [Ssim Fundamentals](/txt/ssim.md)<br/>
&#128193; [Recipes](/txt/recipe/README.md)<br/>
&#128193; [Tutorials](/txt/tut/README.md)<br/>
&#128193; [Ssim Databases](/txt/ssimdb/README.md)<br/>
&#128193; [Executables](/txt/exe/README.md)<br/>
&#128193; [Libraries](/txt/lib/README.md)<br/>
&#128193; [Protocols](/txt/protocol/README.md)<br/>
&#128193; [Scripts](/txt/script/README.md)<br/>

<!-- dev.mdmark  mdmark:TOC  state:END_AUTO  param:Toc -->

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

