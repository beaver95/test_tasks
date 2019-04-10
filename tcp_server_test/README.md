# configure:
change entry of server.conf in format below:

*src_ip src_port con_lim*

* src_ip   - ipv4 address to accept request by proxy, 0.0.0.0 for any;
* src_port - port which used to connect proxy sever;
* con_lim  - maximum number of connections;

# run:
bin/server 2>/dev/null