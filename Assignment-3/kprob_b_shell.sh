#! /bin/bash

: '
$* &
pid=$!
sudo insmod kprobes_b.ko u_pid=$pid
wait $pid
sudo rmmod kprobes_b
dmesg | tail -90
'

: '
sudo insmod kprobes_b.ko u_pid=4577
wait $pid
sudo rmmod kprobes_b
dmesg | tail -90
sudo rmmod kprobes_b
dmesg | tail -200
'

pid=`pgrep sysbench | head -n 1`
sudo insmod kprobes_b.ko u_pid=$pid


