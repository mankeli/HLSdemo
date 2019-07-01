sudo ip addr flush dev eth0
sudo ip addr add 192.168.0.1/24 dev eth0
sudo busybox udhcpd -f udhcpd.conf

