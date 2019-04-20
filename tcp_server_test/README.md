# configure:
change entry of server.conf in format below:

*src_ip src_port con_lim eol_type*

* src_ip   - ipv4 address to accept request by proxy, 0.0.0.0 for any;
* src_port - port which used to connect proxy sever;
* con_lim  - maximum number of connections;
* eol_type - type of line ending (0 - CR, 1 - LF, 2 - CRLF);

# run:
bin/server 2>/dev/null