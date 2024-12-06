#!/bin/bash

set -ex

if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

./db_bench --benchmarks=fillrandom,updaterandom --fs_uri=engineswap://io_uring --compression_type=none -histogram &
#watch -n 1 ls &
PID=$!
echo "hei! db_bench has started $PID"
sudo bpftrace /home/joachim/Desktop/GitHub/rocksdb-VE/syscallOverheadMeasurement.bt -p $PID
#sudo bpftrace -p $PID -e 'tracepoint:syscalls:sys_enter_close { printf("%s is opening.\n", comm); }'

pkill db_bench
