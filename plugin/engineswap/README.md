# Engineswap plugin for rocksdb:

Engineswap is a storage backend for rocksdb which will be able to use io_uring and posix on a configurable parameter

Example compilation:
```bash
#!/bin/bash
export EXTRA_CXXFLAGS="-DGFLAGS=1 -lgflags -fomit-frame-pointer"
export EXTRA_CFLAGS=-DGFLAGS=1
export EXTRA_LDFLAGS="-lgflags"

pushd /home/user/rocksdb-VE
DISABLE_WARNING_AS_ERROR=1 DEBUG_LEVEL=0 ROCKSDB_PLUGINS="engineswap" make -j $(nproc) static_lib db_bench #--dry-run
popd

```
Example execution:\
```bash
$ROCKSDB_PATH/db_bench --benchmarks=fillrandom --fs_uri=engineswap://$ENGINE \
      --db=/local/nvmevirt/ \
      --compression_type=none -histogram \
      --disable_auto_compactions \
      --max_background_compactions=0 \
      -cache_size=0 \
      --num=50000000
```
$Engine can be anyone of the following:
 * io_uring - uses vectorised io
 * io_uring_nv - uses non vectorised io
 * posix
 * libaio
 * io_uring_nv_sqp - uses submission queue polling
 * dummy - uses default filesystem, but prints out all the calls it receives.
