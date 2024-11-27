#include "posixengine.h"
#include "filedummies.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <sys/stat.h>
#include <iostream>

namespace rocksdb{
    //uncomment these to make some warnings :)

    IOStatus NewRandomAccessFilePosix(const std::string& f, 
                           const FileOptions& file_opts,
                           std::unique_ptr<FSRandomAccessFile>* r,
                           IODebugContext* dbg)
    {
        result->reset();
    IOStatus s = IOStatus::OK();
    int fd;
    int flags = cloexec_flags(O_RDONLY, &options);

    if (options.use_direct_reads && !options.use_mmap_reads) {
#if !defined(OS_MACOSX) && !defined(OS_OPENBSD) && !defined(OS_SOLARIS)
      flags |= O_DIRECT;
      TEST_SYNC_POINT_CALLBACK("NewRandomAccessFile:O_DIRECT", &flags);
#endif
    }

    do {
      IOSTATS_TIMER_GUARD(open_nanos);
      fd = open(fname.c_str(), flags, GetDBFileMode(allow_non_owner_access_));
    } while (fd < 0 && errno == EINTR);
    if (fd < 0) {
      s = IOError("While open a file for random read", fname, errno);
      return s;
    }
    SetFD_CLOEXEC(fd, &options);

    if (options.use_mmap_reads && false) {
      // Use of mmap for random reads has been removed because it
      // kills performance when storage is fast.
      // Use mmap when virtual address-space is plentiful.
      uint64_t size;
      IOOptions opts;
      s = GetFileSize(fname, opts, &size, nullptr);
      if (s.ok()) {
        void* base = mmap(nullptr, size, PROT_READ, MAP_SHARED, fd, 0);
        if (base != MAP_FAILED) {
          result->reset(
              new PosixMmapReadableFile(fd, fname, base, size, options));
        } else {
          s = IOError("while mmap file for read", fname, errno);
          close(fd);
        }
      } else {
        close(fd);
      }
    } else {
      if (options.use_direct_reads && !options.use_mmap_reads) {

      }
      result->reset(new RandomAccessFilePosix(
          fname, fd, GetLogicalBlockSizeForReadIfNeeded(options, fname, fd),
          options
#if defined(ROCKSDB_IOURING_PRESENT)
          ,
          !IsIOUringEnabled() ? nullptr : thread_local_io_urings_.get()
#endif
              ));
        }
    }

    IOStatus NewSequentialFilePosix(const std::string& f, 
                            const FileOptions& file_opts,
                            std::unique_ptr<FSSequentialFile>* r,
                            IODebugContext* dbg)
    {
        result->reset();
        int fd = -1;
        int flags = cloexec_flags(O_RDONLY, &options);
        FILE* file = nullptr;

        if (options.use_direct_reads && !options.use_mmap_reads) {
    #if !defined(OS_MACOSX) && !defined(OS_OPENBSD) && !defined(OS_SOLARIS)
        flags |= O_DIRECT;
        TEST_SYNC_POINT_CALLBACK("NewSequentialFile:O_DIRECT", &flags);
    #endif
        }

        do {
        IOSTATS_TIMER_GUARD(open_nanos);
        fd = open(fname.c_str(), flags, GetDBFileMode(allow_non_owner_access_));
        } while (fd < 0 && errno == EINTR);
        if (fd < 0) {
        return IOError("While opening a file for sequentially reading", fname,
                        errno);
        }

        SetFD_CLOEXEC(fd, &options);

        if (options.use_direct_reads && !options.use_mmap_reads) {
            std::cout << "using direct reads\n";
        } else {
        do {
            IOSTATS_TIMER_GUARD(open_nanos);
            file = fdopen(fd, "r");
        } while (file == nullptr && errno == EINTR);
        if (file == nullptr) {
            close(fd);
            return IOError("While opening file for sequentially read", fname,
                        errno);
        }
        }
        result->reset(new SequentialFilePosix(
            fname, file, fd, GetLogicalBlockSizeForReadIfNeeded(options, fname, fd),
            options));
        return IOStatus::OK();
    }

    virtual IOStatus OpenWritableFile(const std::string& fname,
                                    const FileOptions& options, bool reopen,
                                    std::unique_ptr<FSWritableFile>* result,
                                    IODebugContext* /*dbg*/) {
        result->reset();
        IOStatus s;
        int fd = -1;
        int flags = (reopen) ? (O_CREAT | O_APPEND) : (O_CREAT | O_TRUNC);
        // Direct IO mode with O_DIRECT flag or F_NOCAHCE (MAC OSX)
        if (options.use_direct_writes && !options.use_mmap_writes) {
        // Note: we should avoid O_APPEND here due to ta the following bug:
        // POSIX requires that opening a file with the O_APPEND flag should
        // have no affect on the location at which pwrite() writes data.
        // However, on Linux, if a file is opened with O_APPEND, pwrite()
        // appends data to the end of the file, regardless of the value of
        // offset.
        // More info here: https://linux.die.net/man/2/pwrite
        flags |= O_WRONLY;
    #if !defined(OS_MACOSX) && !defined(OS_OPENBSD) && !defined(OS_SOLARIS)
        flags |= O_DIRECT;
    #endif
        TEST_SYNC_POINT_CALLBACK("NewWritableFile:O_DIRECT", &flags);
        } else if (options.use_mmap_writes) {
        // non-direct I/O
        flags |= O_RDWR;
        } else {
        flags |= O_WRONLY;
        }

        flags = cloexec_flags(flags, &options);

        do {
        IOSTATS_TIMER_GUARD(open_nanos);
        fd = open(fname.c_str(), flags, GetDBFileMode(allow_non_owner_access_));
        } while (fd < 0 && errno == EINTR);

        if (fd < 0) {
        s = IOError("While open a file for appending", fname, errno);
        return s;
        }
        SetFD_CLOEXEC(fd, &options);

        if (options.use_mmap_writes) {
        MaybeForceDisableMmap(fd);
        }
        if (options.use_mmap_writes && !forceMmapOff_) {
        result->reset(new PosixMmapFile(fname, fd, page_size_, options));
        } else if (options.use_direct_writes && !options.use_mmap_writes) {
    #ifdef OS_MACOSX
        if (fcntl(fd, F_NOCACHE, 1) == -1) {
            close(fd);
            s = IOError("While fcntl NoCache an opened file for appending", fname,
                        errno);
            return s;
        }
    #elif defined(OS_SOLARIS)
        if (directio(fd, DIRECTIO_ON) == -1) {
            if (errno != ENOTTY) {  // ZFS filesystems don't support DIRECTIO_ON
            close(fd);
            s = IOError("While calling directio()", fname, errno);
            return s;
            }
        }
    #endif
        result->reset(new PosixWritableFile(
            fname, fd, GetLogicalBlockSizeForWriteIfNeeded(options, fname, fd),
            options));
        } else {
        // disable mmap writes
        EnvOptions no_mmap_writes_options = options;
        no_mmap_writes_options.use_mmap_writes = false;
        result->reset(
            new PosixWritableFile(fname, fd,
                                    GetLogicalBlockSizeForWriteIfNeeded(
                                        no_mmap_writes_options, fname, fd),
                                    no_mmap_writes_options));
        }
        return s;
    }

    IOStatus NewWritableFilePosix(const std::string& f, 
                            const FileOptions& file_opts,
                            std::unique_ptr<FSWritableFile>* r,
                            IODebugContext* dbg)
    {
        return OpenWritableFile(fname, options, false, result, dbg);   
    }

    // RandomAccessFileDummy::GetRequiredBufferAlignment seems implemented
    // RandomAccessFileDummy::Prefetch perhaps we can get away with not implementing it?

    IOStatus RandomAccessFilePosix::Read(uint64_t offset, size_t n, const IOOptions& options,
				Slice* result, char* scratch,
				IODebugContext* dbg) const
    {
        std::cout << "RandomAccessFilePosix::Read\n";
    }

    IOStatus SequentialFilePosix::Read(size_t n, const IOOptions& opts, Slice* result, char* scratch,
				IODebugContext* dbg) 
    {
        std::cout << "SequentialFilePosix::Read\n";
        assert(result != nullptr && !use_direct_io());
        IOStatus s = IOStatus::OK();
        size_t r = 0;
        do {
            clearerr(this->file);
            r = fread_unlocked(scratch, 1, n, this->file);
        } while (r == 0 && ferror(this->file) && errno == EINTR);
        *result = Slice(scratch, r);
        if (r < n) {
            if (feof(this->file)) {
            // We leave status as ok if we hit the end of the file
            // We also clear the error so that the reads can continue
            // if a new data is written to the file
            clearerr(this->file);
            } else {
            // A partial read with an error: return a non-ok status
                s = IOStatus::IOError("While reading file sequentially");
            }
        }
        return s;   
    }


    IOStatus WritableFilePosix::Append(const Slice& data, const IOOptions& /*opts*/,
                                   IODebugContext* /*dbg*/) {
        if (use_direct_io()) {
            assert(IsSectorAligned(data.size(), GetRequiredBufferAlignment()));
            assert(IsSectorAligned(data.data(), GetRequiredBufferAlignment()));
        }
        const char* src = data.data();
        size_t nbytes = data.size();

        if (!PosixWrite(fd_, src, nbytes)) {
            return IOError("While appending to file", filename_, errno);
        }

        filesize_ += nbytes;
        return IOStatus::OK();
    }

    IOStatus WritableFilePosix::Close(const IOOptions& /*options*/, IODebugContext* /*dbg*/){
        IOStatus s;

        size_t block_size;
        size_t last_allocated_block;
        GetPreallocationStatus(&block_size, &last_allocated_block);
        TEST_SYNC_POINT_CALLBACK("PosixWritableFile::Close", &last_allocated_block);
        if (last_allocated_block > 0) {
            // trim the extra space preallocated at the end of the file
            // NOTE(ljin): we probably don't want to surface failure as an IOError,
            // but it will be nice to log these errors.
            int dummy __attribute__((__unused__));
            dummy = ftruncate(fd_, filesize_);
        #if defined(ROCKSDB_FALLOCATE_PRESENT) && defined(FALLOC_FL_PUNCH_HOLE)
            // in some file systems, ftruncate only trims trailing space if the
            // new file size is smaller than the current size. Calling fallocate
            // with FALLOC_FL_PUNCH_HOLE flag to explicitly release these unused
            // blocks. FALLOC_FL_PUNCH_HOLE is supported on at least the following
            // filesystems:
            //   XFS (since Linux 2.6.38)
            //   ext4 (since Linux 3.0)
            //   Btrfs (since Linux 3.7)
            //   tmpfs (since Linux 3.5)
            // We ignore error since failure of this operation does not affect
            // correctness.
            struct stat file_stats;
            int result = fstat(fd_, &file_stats);
            // After ftruncate, we check whether ftruncate has the correct behavior.
            // If not, we should hack it with FALLOC_FL_PUNCH_HOLE
            if (result == 0 &&
                (file_stats.st_size + file_stats.st_blksize - 1) /
                        file_stats.st_blksize !=
                    file_stats.st_blocks / (file_stats.st_blksize / 512)) {
            IOSTATS_TIMER_GUARD(allocate_nanos);
            if (allow_fallocate_) {
                fallocate(fd_, FALLOC_FL_KEEP_SIZE | FALLOC_FL_PUNCH_HOLE, filesize_,
                        block_size * last_allocated_block - filesize_);
            }
            }
        #endif
        }

        if (close(fd_) < 0) {
            s = IOError("While closing file after writing", filename_, errno);
        }
        fd_ = -1;
        return s;
    }

    IOStatus WritableFilePosix::Flush(const IOOptions& options, IODebugContext* dbg){
        //this is implemented believe it or not
        return IOStatus::OK();
    }
    
    IOStatus WritableFilePosix::Sync(const IOOptions& options,
                IODebugContext* dbg)
    {
        #ifdef HAVE_FULLFSYNC
        if (::fcntl(fd_, F_FULLFSYNC) < 0) {
            return IOError("while fcntl(F_FULLFSYNC)", filename_, errno);
        }
        #else   // HAVE_FULLFSYNC
        if (fdatasync(fd_) < 0) {
            return IOError("While fdatasync", filename_, errno);
        }
        #endif  // HAVE_FULLFSYNC
        return IOStatus::OK();
    }
    
    uint64_t WritableFilePosix::GetFileSize(const IOOptions& /*options*/,
                       IODebugContext* /*dbg*/)
    {
        return this->filesize;
    }

    IOStatus WritableFilePosix::Fsync(const IOOptions& options, IODebugContext* dbg)
    {
        IOStatus PosixWritableFile::Fsync(const IOOptions& /*opts*/,
                                  IODebugContext* /*dbg*/) {
        #ifdef HAVE_FULLFSYNC
        if (::fcntl(fd_, F_FULLFSYNC) < 0) {
            return IOError("while fcntl(F_FULLFSYNC)", filename_, errno);
        }
        #else   // HAVE_FULLFSYNC
        if (fsync(fd_) < 0) {
            return IOError("While fsync", filename_, errno);
        }
        #endif  // HAVE_FULLFSYNC
        return IOStatus::OK();
        }
    }

    IOStatus WritableFilePosix::RangeSync(uint64_t /*offset*/, uint64_t /*nbytes*/,
                     const IOOptions& options, IODebugContext* dbg) {
        return FSWritableFile::RangeSync(0, 0, options, dbg);
    }

    size_t WritableFilePosix::GetRequiredBufferAlignment() const 
    { 
        return logical_sector_size_; 
    }

    IOStatus WritableFilePosix::RangeSync(uint64_t /*offset*/, uint64_t /*nbytes*/,
                     const IOOptions& options, IODebugContext* dbg) {
        #ifdef ROCKSDB_RANGESYNC_PRESENT
        assert(offset <= static_cast<uint64_t>(std::numeric_limits<off_t>::max()));
        assert(nbytes <= static_cast<uint64_t>(std::numeric_limits<off_t>::max()));
        if (sync_file_range_supported_) {
            int ret;
            if (strict_bytes_per_sync_) {
            // Specifying `SYNC_FILE_RANGE_WAIT_BEFORE` together with an offset/length
            // that spans all bytes written so far tells `sync_file_range` to wait for
            // any outstanding writeback requests to finish before issuing a new one.
            ret =
                sync_file_range(fd_, 0, static_cast<off_t>(offset + nbytes),
                                SYNC_FILE_RANGE_WAIT_BEFORE | SYNC_FILE_RANGE_WRITE);
            } else {
            ret = sync_file_range(fd_, static_cast<off_t>(offset),
                                    static_cast<off_t>(nbytes), SYNC_FILE_RANGE_WRITE);
            }
            if (ret != 0) {
            return IOError("While sync_file_range returned " + std::to_string(ret),
                            filename_, errno);
            }
            return IOStatus::OK();
        }
        #endif  // ROCKSDB_RANGESYNC_PRESENT
        return FSWritableFile::RangeSync(offset, nbytes, opts, dbg);
    }

    void WritableFilePosix::SetPreallocationBlockSize(size_t size) {
        FSWritableFile::SetPreallocationBlockSize(size);
    }

    void WritableFilePosix::SetWriteLifeTimeHint(Env::WriteLifeTimeHint hint) 
    { 
        write_hint_ = hint; 
    }

    bool WritableFilePosix::use_direct_io() const 
    { 
        return false; 
    }


    // RandomAccessFileDummy::Read 
    
    // SequentialFileDummy::Read 
    
    // WritableFileDummy::Append 
    // WritableFileDummy::Close 
    // WritableFileDummy::Flush 
    // WritableFileDummy::Fsync (implement sync instead)
    // WritableFileDummy::GetFileSize 

    // WritableFileDummy::RangeSync 
    // WritableFileDummy::SetPreallocationBlockSize 
    // WritableFileDummy::SetWriteLifeTimeHint 
    // WritableFileDummy::Sync 
    // WritableFileDummy::use_direct_io 
}

