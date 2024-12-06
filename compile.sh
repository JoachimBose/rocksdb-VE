#!/bin/bash
export EXTRA_LDFLAGS="-lgflags"
export EXTRA_CXXFLAGS="-DGFLAGS=1 -lgflags"
export EXTRA_CFLAGS=-DGFLAGS=1
export LD_LIBRARY_PATH=/local/data/ade204/rocksdb/lib
export LIBRARY_PATH=/local/data/ade204/rocksdb/lib
export CPATH=/local/data/ade204/rocksdb/include
DISABLE_WARNING_AS_ERROR=1 DEBUG_LEVEL=0 ROCKSDB_PLUGINS="engineswap" make -j 20 static_lib db_bench
