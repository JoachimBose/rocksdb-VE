#!/bin/bash

set -x

if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

ROCKSDB_PATH=./
FLAMEGRAPH_LOC=../Flamegraph
PERF_LOC="/lib/linux-tools/5.15.0-127-generic/perf"


array=( io_uring_nv posix )
for i in "${array[@]}"
do
  /home/user/resetNvmevirt.sh > /dev/null
  rm -f /local/nvmevirt/*
  PID=asid

  ENGINE=$i
  trap 'echo "exiting script" && pkill db_bench' EXIT

  #echo "USDT probes available:"
  #bpftrace -l "usdt:./db_bench:*"

  $ROCKSDB_PATH/db_bench --benchmarks=fillrandom --fs_uri=engineswap://$ENGINE \
      --db=/local/nvmevirt/ \
      --compression_type=none -histogram \
      --disable_auto_compactions \
      --max_background_compactions=0 \
      -cache_size=0 \
      --num=50000000 \
      > ./perf-out$ENGINE.txt &
  #watch -n 1 ls &
  PID=$!
  echo "hei! db_bench has started $PID"

  #usdt:/home/user/rocksdb-VE/db_bench:libaio:write,
  #usdt:/home/user/rocksdb-VE/db_bench:posix:write,
  #usdt:/home/user/rocksdb-VE/db_bench:io_uring:write,
  #usdt:/home/user/rocksdb-VE/db_bench:io_uring_nv:write

  #get the tid of the main thread
  TID=$(bpftrace -p $PID -e \
  'usdt:/home/user/rocksdb-VE/db_bench:libaio:write,
  usdt:/home/user/rocksdb-VE/db_bench:posix:write,
  usdt:/home/user/rocksdb-VE/db_bench:io_uring:write,
  usdt:/home/user/rocksdb-VE/db_bench:io_uring_nv:write
  { printf("%d\n", tid); 
    exit();}' | tr -cd '[[:digit:]]')

  echo "recording started.. tid=$TID"

  $PERF_LOC record -F 2000 -p $PID -t $TID -g -- sleep 50
  echo "perf done, making svg"
  $PERF_LOC script > out$ENGINE.perf
  $FLAMEGRAPH_LOC/stackcollapse-perf.pl out$ENGINE.perf > out$ENGINE.folded
  cp out$ENGINE.folded outCp$ENGINE.folded

  # setting the fontsize to 0.1 allows us to remove the text, making it easier to show in latex.

  $FLAMEGRAPH_LOC/flamegraph.pl out$ENGINE.folded --color=rocksdb --fontsize 0.1 > /home/user/svgs/auto-shape-$ENGINE.svg
  $FLAMEGRAPH_LOC/flamegraph.pl outCp$ENGINE.folded --color=rocksdb > /home/user/svgs/auto-$ENGINE.svg
  pkill db_bench
  kill $PID
done
echo "script finished"

