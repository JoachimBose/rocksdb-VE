#!/bin/bash
DISABLE_WARNING_AS_ERROR=1 DEBUG_LEVEL=0 ROCKSDB_PLUGINS="engineswap" make -j 20 static_lib db_bench
