## gendb - Tables amc derives from the schema
<a href="#gendb"></a>
Every table here is amc's output.  amc computes the rows from the rest of the
schema on each run and rewrites the files to match the code it generated, so an
edit to a row lasts only until the next run.  Change the schema and run `amc`.

Other tools read these tables: `src_func` reads `gendb.cppsym` and
`gendb.ctypelen`, `acr_in` reads `gendb.dispsig`, `apm` reads `gendb.cppsym`,
and the gateway serves `gendb.payloadhdr`, `gendb.msg` and `gendb.msgfield` to
clients that decode messages.  amc itself reads none of them.
