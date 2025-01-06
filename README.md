## RocksDB: A Persistent Key-Value Store for Flash and RAM Storage

[![CircleCI Status](https://circleci.com/gh/facebook/rocksdb.svg?style=svg)](https://circleci.com/gh/facebook/rocksdb)

RocksDB is developed and maintained by Facebook Database Engineering Team.
It is built on earlier work on [LevelDB](https://github.com/google/leveldb) by Sanjay Ghemawat (sanjay@google.com)
and Jeff Dean (jeff@google.com)

This code is a library that forms the core building block for a fast
key-value server, especially suited for storing data on flash drives.
It has a Log-Structured-Merge-Database (LSM) design with flexible tradeoffs
between Write-Amplification-Factor (WAF), Read-Amplification-Factor (RAF)
and Space-Amplification-Factor (SAF). It has multi-threaded compactions,
making it especially suitable for storing multiple terabytes of data in a
single database.

Start with example usage here: https://github.com/facebook/rocksdb/tree/main/examples

See the [github wiki](https://github.com/facebook/rocksdb/wiki) for more explanation.

The public interface is in `include/`.  Callers should not include or
rely on the details of any other header files in this package.  Those
internal APIs may be changed without warning.

Questions and discussions are welcome on the [RocksDB Developers Public](https://www.facebook.com/groups/rocksdb.dev/) Facebook group and [email list](https://groups.google.com/g/rocksdb) on Google Groups.

## License

RocksDB is dual-licensed under both the GPLv2 (found in the COPYING file in the root directory) and Apache 2.0 License (found in the LICENSE.Apache file in the root directory).  You may select, at your option, one of the above-listed licenses.

# Engineswap plugin for rocksdb:

Engineswap is a storage backend for rocksdb which will be able to use io_uring and posix based on a configurable parameter. The code is in rocksdb-VE/plugin/engineswap/.

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
$ENGINE can be anyone of the following:
 * io_uring - uses vectorised io
 * io_uring_nv - uses non vectorised io
 * posix
 * libaio
 * io_uring_nv_sqp - uses submission queue polling
 * dummy - uses default filesystem, but prints out all the calls it receives.
