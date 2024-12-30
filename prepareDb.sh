#!/bin/bash

pushd /home/user/rocksdb-VE
figlet Preparing db
./db_bench --benchmarks=fillseq --fs_uri=engineswap://io_uring --compression_type=none -histogram --db /local/nvmevirt --num 50000000
popd
