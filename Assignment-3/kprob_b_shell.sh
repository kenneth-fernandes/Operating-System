#! /bin/bash

: '
$* &
pid=$!
sudo insmod pf_probe_B.ko u_pid=$pid
wait $pid
sudo rmmod pf_probe_B
dmesg | tail -90
'

: '
sudo insmod pf_probe_B.ko u_pid=4577
wait $pid
sudo rmmod pf_probe_B
dmesg | tail -90
sudo rmmod pf_probe_B
dmesg | tail -200
'

pid=`pgrep firefox | head -n 1`
sudo insmod pf_probe_B.ko u_pid=$pid


