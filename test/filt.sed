s/(tsc|signature):[^ ]+/\1:***/g
s/^(lib_ws.server_(stopped|started)  server:[^:]*:)[0-9]*/\1***/g
/netio/s/([0-9]+\.[0-9]+\.[0-9]+\.[0-9]:)[0-9]+/\1*/g
