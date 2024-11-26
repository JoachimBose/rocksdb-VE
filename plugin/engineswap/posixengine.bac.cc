#include "posixengine.h"
#include "filedummies.h"

#include "rocksdb/file_system.h"
#include "rocksdb/status.h"
#include "rocksdb/io_status.h"

#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

namespace rocksdb{

#define FILEMODE O_RDWR
#define FOPEN_MODE "r+"

class PosixFile{
  public:
    FILE* fp;
    int fd;
    std::string fname;
    size_t logical_sector_size;
    FileOptions file_opts;
    
    //if anyone knows how to do this better, lmk
    PosixFile(std::string nfname, FILE* nfp, int nfd, size_t nlss, const FileOptions& nfile_opts){
      fp = nfp;
      fd = nfd;
      fname = nfname;
      logical_sector_size = nlss;
      file_opts = nfile_opts;
    }
};

size_t getBlkSize(int fd){
  struct stat buf;
  fstat(fd, &buf);
  return buf.st_size;
}

PosixFile NewPosixFile(const std::string& f, 
                           const FileOptions& file_opts,
                           IODebugContext* dbg)
{
  //Todo:
  // 1. make open call, remember EINTR 
  //    open(fname.c_str(), flags, GetDBFileMode(allow_non_owner_access_));
  // 2. do cloexec stuff
  // 3. check mmap reads
  // 4. set the result
  // 5. return IOstatus::OK()
  int flags = O_CLOEXEC | O_CREAT;
  int fd = 0;
  do {
    fd = open(f.c_str(), flags, O_RDWR); 
  } while(fd < 0 && errno == EINTR);
  
  if (fd > 0) {
    FILE* fp = fdopen(fd, FOPEN_MODE);
    dbg->msg = ("posixFileError");
    return PosixFile(f, fp, fd, getBlkSize(fd), file_opts);
  }
  return PosixFile(f, NULL, fd, 0, file_opts);
}

IOStatus NewRandomAccessFilePosix(const std::string& f, 
                           const FileOptions& file_opts,
                           std::unique_ptr<FSRandomAccessFile>* r,
                           IODebugContext* dbg)
{
  PosixFile posix_file = NewPosixFile(f, file_opts, dbg); 
  if (posix_file.fd < 0)
  {
    return IOStatus::IOError("newSequentialPosixFile");
  }
  // r->reset(new RandomAccessFilePosix(f, 
  //   posix_file.fp,
  //   posix_file.fd,
  //   posix_file.logical_sector_size,
  //   file_opts));
  // r->reset(new SequentialFileDummy());
  return IOStatus::OK();
}

IOStatus NewSequentialFilePosix(const std::string& f, 
                          const FileOptions& file_opts,
                          std::unique_ptr<FSSequentialFile>* r,
                          IODebugContext* dbg)
{
  PosixFile posix_file = NewPosixFile(f, file_opts, dbg); 
  if (posix_file.fd < 0)
  {
    return IOStatus::IOError("newSequentialPosixFile");
  }
  // r->reset(new SequentialFilePosix(f, 
  //   posix_file.fp,
  //   posix_file.fd,
  //   posix_file.logical_sector_size,
  //   file_opts));
  // r->reset(new SequentialFileDummy());
  return IOStatus::OK();
}

IOStatus NewWritableFilePosix(const std::string& f, 
                          const FileOptions& file_opts,
                          std::unique_ptr<FSWritableFile>* r,
                          IODebugContext* dbg)
  {
    PosixFile posix_file = NewPosixFile(f, file_opts, dbg); 
    if (posix_file.fd < 0)
    {
      return IOStatus::IOError("newSequentialPosixFile");
    }
    // r->reset(new WritableFilePosix(f, 
    //   posix_file.fp,
    //   posix_file.fd,
    //   posix_file.logical_sector_size,
    //   file_opts));
    // r->reset(new SequentialFileDummy());
    return IOStatus::OK();
  }
} //namespace rocksdb

