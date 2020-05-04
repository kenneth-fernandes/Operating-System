#! /bin/bash

: '
$* &
pid=$!
sudo insmod kprobes_b.ko u_pid=$pid
wait $pid
sudo rmmod kprobes_b
dmesg | tail -90
'


sudo insmod kprobes_b.ko u_pid=5444
wait $pid
sudo rmmod kprobes_b
dmesg | tail -90

