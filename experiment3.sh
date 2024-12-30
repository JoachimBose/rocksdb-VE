#!/bin/bash

set -ex

if [ "$EUID" -ne 0 ]
    then echo "Please run as root"
    exit
fi

ROCKSDB_PATH=/home/user/rocksdb-VE

trap 'sudo pkill bpftrace' EXIT

array=( posix io_uring_nv )
for i in "${array[@]}"
do
    ./resetNvmevirt.sh
    PID=asid

    ENGINE=$i

    #echo "USDT probes available:"
    #bpftrace -l "usdt:./db_bench:*"
    bpftrace smalltrace-$ENGINE.bt > .//smalltrace-bpf-$ENGINE.txt &
    BPFTRACE_PID=$!
    echo "bpftrace pid $BPFTRACE_PID"
    sleep 3

    $ROCKSDB_PATH/db_bench --benchmarks=fillrandom --fs_uri=engineswap://$ENGINE \
    --db=/local/nvmevirt/ \
    --compression_type=none -histogram \
    --disable_auto_compactions \
    --max_background_compactions=0 \
    -cache_size=0 \
    --num=50000000 \
    --disable_wal=1 \
    > ./smalltrace-out-$ENGINE.txt

    kill $BPFTRACE_PID

#get the tid of the main thread
done
echo "script finished"

