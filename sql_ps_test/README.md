# configure:
change entry of proxy.conf in format below:

*src_ip src_port dst_ip dst_port.*

* src_ip   - ipv4 address to accept request by proxy, 0.0.0.0 for any;
* src_port - port which used to connect proxy sever;
* dst_ip   - ipv4 address where requests will be redirected;
* dst_port - port where requests will be redirected.

# run:
bin/serdes 2>/dev/null

# result:
see intercepted requests in "sql.log" file