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

## Functions used by Fillseq,readrandom
* NewDirectory
* NewLogger
* NewRandomAccessFile
* NewSequentialFile
* NewWritableFile
* NumFileLinks
* OptimizeForCompactionTableRead
* OptimizeForCompactionTableWritea
* OptimizeForLogWrite
* OptimizeForManifestRead
* OptimizeForManifestWrite
* LockFile
* GetAbsolutePath
* GetChildren
* GetFileSize
* GetTestDirectory
* FileExists
* DeleteDir
* DeleteFile
* CreateDirIfMissing
* RenameFile
* SupportedOps
* UnlockFile



## DB_bench options to keep in mind
See https://github.com/stonet-research/storage-systems-wiki-reading-list/wiki/RocksDB-notes
for more info
* -async_io default: false
* -histogram show hist
* -num number of kv pairs
* -key_size
* -value_size
* -io_uring_enabled 
* -keyrange_dist_X where x is a parameter
* -perf_level
* -report_file
(If true, enable the use of IO uring if the platform supports it) type: bool default: true

## Benchmarks:
* fillseq
* updaterandom
