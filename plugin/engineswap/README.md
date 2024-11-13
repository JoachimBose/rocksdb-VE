# Engineswap plugin for rocksdb:

Engineswap is a storage backend for rocksdb which will be able to use io_uring and posix on a configurable parameter

## Installing

After installing the rocksdb and getting db_bench to run this is how one would install engineswap:

```bash
cd rocksdb # in the current installation of rocksdb
cd plugin/
git clone https:www.github.com/JoachimBose/engineswap
cd ../..

```
