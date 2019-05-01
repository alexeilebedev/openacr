## MariaDB integration

All ssim tables are compatible with MariaDB. The namespace (ns) corresponds to a database,
and ssimfile corresponds to a table.
Tools mysql2ssim and ssim2mysql perform conversion to/from ssim schema to SQL, and
acr_my can set up a local MariaDB server process and iteract with
it via a socket.

Here are some useful commands:
Start a local MariaDB server:

    acr_my -start dmmeta

Connect to the local server on the command line:

    acr_my -shell

From here, you can issue SQL commands:

    select count(*) from dmmeta.field where arg ='u8';

When you exit from the shell, the server keeps running.
You can stop the server and save changes:

    acr_my -stop

Or stop the server, discarding changes:

    acr_my -abort

You can also use acr -my as a shortcut for acr_my -start -shell -stop:

    echo 'update thash set unique='N' where field like "acr.%"' | acr -my -fldfunc dmmeta.%
    amc

The table dmmeta.sqltype allows ssim2mysql to map ctypes to SQL types so that
round tripping can occur without loss.

