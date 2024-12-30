#!/bin/bash

array=( libaio io_uring posix )
for i in "${array[@]}"
do
    rm -rf /local/nvmevirt/*

    source ./prepareDb.sh

    ENGINE=$i
    ./db_bench --benchmarks=mixgraph,stats,levelstats --fs_uri=engineswap://$ENGINE --compression_type=none -histogram \
    --db=/local/nvmevirt \
    --use_existing_db=1 \
    --cache_size=268435456 \
    --key_dist_a=0.002312 -key_dist_b=0.3467  \
    --keyrange_dist_a=14.18  \
    --keyrange_dist_b=-2.917  \
    --keyrange_dist_c=0.0164  \
    --keyrange_dist_d=-0.08082 \
    --keyrange_num=30  \
    --value_k=0.2615  \
    --value_sigma=25.45 \
    --iter_k=2.517  \
    --iter_sigma=14.236  \
    --mix_get_ratio=0.83 \
    --mix_put_ratio=0.14  \
    --mix_seek_ratio=0.03  \
    --sine_mix_rate_interval_milliseconds=5000  \
    --sine_a=1000 \
    --sine_b=0.000073  \
    --sine_d=4500  \
    --perf_level=2  \
    --reads=420000000 \
    --num=50000000 \
    --key_size=48 \
    --statistics=1 \
    --stats_interval_seconds=1 \
    --report_interval_seconds=60 \
    --report_file=/home/user/simple-bench-out-$ENGINE.csv \
    --threads 8
    > /home/user/simple-bench-out-$ENGINE.txt
    #-reads=420000000  \
    #-num=50000000  \
    #-key_size=48
done

