#include <liburing.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <iostream>

#define QUEUE_ITEMS 1

class Slice{
private:
    size_t size_;
    void * data_;
public:
    Slice(void* ndata, size_t nsize){
        size_ = nsize;
        data_ = ndata;
    }

    size_t size(){
        return size_;
    }
    void* data(){
        return data_;
    }
};

class IouRing{
private:
  struct io_uring ring;
public:
  IouRing(){
    int ret = io_uring_queue_init(QUEUE_ITEMS, &ring, 0);
    if(ret != 0){
        fprintf(stderr, "queue init failed %i \n errno = %i\n errno: %s\n", ret, errno, strerror(errno));
    }
  }
  ~IouRing(){
    io_uring_queue_exit(&ring);
  }
  int IouRingRead(size_t n, Slice* result, char* scratch, int block_size, int fd)
    {
        // submit requests per block
        off_t offset = 0;
        size_t bytes_remaining = n;
        int current_block = 0;

        int blocks = (int)n / block_size;
        if (n % block_size) blocks++;
        
        //one iovec per request
        struct iovec* iovecvec = (struct iovec*)calloc(blocks, sizeof(struct iovec));
        if (!iovecvec)
        {
        std::cerr << "iovecvec calloc failed \n";
        return -1;
        }
        
        while (bytes_remaining > 0)
        {
        off_t bytes_to_read = bytes_remaining;
        if (bytes_to_read > block_size)
            bytes_to_read = block_size;
        
        iovecvec[current_block].iov_len = bytes_to_read;
        iovecvec[current_block].iov_base = (void*)scratch + offset;
        
        offset += bytes_to_read;
        current_block++;
        bytes_remaining -= bytes_to_read;
        }
        
        struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
        io_uring_prep_readv(sqe, fd, iovecvec, blocks, 0);
        io_uring_sqe_set_data(sqe, iovecvec);
        io_uring_submit(&this->ring);

        struct io_uring_cqe *cqe;
        int ret = io_uring_wait_cqe(&ring, &cqe);
        if (ret < 0) {
            perror("io_uring_wait_cqe");
            io_uring_cqe_seen(&ring, cqe);
            free(iovecvec);
            return 1;
        }
        if (cqe->res < 0) {
            fprintf(stderr, "Async readv failed.\n");
            io_uring_cqe_seen(&ring, cqe);
            free(iovecvec);
        return 1;
        }

        iovecvec = (struct iovec*)io_uring_cqe_get_data(cqe);
        
        *result = Slice(scratch, cqe->res);
        free(iovecvec);
        io_uring_cqe_seen(&ring, cqe); //stuff in the cqe cannot be accessed anymore, 
        return 0;
    } 

    int IouRingWrite(Slice& data, int fd, int block_size)
    {
        int current_block = 0;
        off_t offset = 0;

        int blocks = (int)data.size() / block_size;
        size_t bytes_remaining = data.size();

        if (data.size() % block_size) blocks++;
        
        struct iovec* iovecvec = (struct iovec*)calloc(blocks, sizeof(struct iovec));
        if (!iovecvec)
        {
        std::cerr << "iovecvec calloc failed \n";
        return -1;
        }
        
        while (bytes_remaining > 0)
        {
        off_t bytes_to_write = bytes_remaining;
        if (bytes_to_write > block_size)
            bytes_to_write = block_size;

        iovecvec[current_block].iov_len = bytes_to_write;
        iovecvec[current_block].iov_base = (void*)data.data() + offset;

        offset += bytes_to_write;
        current_block++;
        bytes_remaining -= bytes_to_write;
        }

        struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
        io_uring_prep_writev(sqe, fd, iovecvec, blocks, 0);
        io_uring_sqe_set_data(sqe, iovecvec);
        io_uring_submit(&this->ring);

        struct io_uring_cqe *cqe;
        int ret = io_uring_wait_cqe(&ring, &cqe);
        if (ret < 0) {
            std::cout <<"io_uring_wait_cqe \n";
            io_uring_cqe_seen(&ring, cqe);
            free(iovecvec);
            return -1;
        }
        if (cqe->res < 0) {
            std::cout <<  "Async writev failed.\n";
            io_uring_cqe_seen(&ring, cqe);
            free(iovecvec);
            return -1;
        }
        // std::cout << "write result: " << cqe->res << " target " << data.size();
        io_uring_cqe_seen(&ring, cqe);
        free(iovecvec);
        return 0;
    }

};

int main(int argc, char* argv[]){
    IouRing io_ring = IouRing();
    char buf[512];
    Slice data = Slice(nullptr, 0);

    if (data.size())
    {
        std::cout << "size initialised\n";
    }

    int fd = open("/home/joachim/Desktop/GitHub/rocksdb-VE/plugin/engineswap/myfile.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if(fd < 0){
        std::cout << "open failed " << fd << " errno: " << errno << "\n";
        return 0;
    }
    std::cout << "fd: " << fd << "\n";

    io_ring.IouRingRead(511, &data, buf, BLOCK_SIZE, STDIN_FILENO);
    char writedata[512] = "ioioioirioiwqrojsi";
    Slice writeslice = Slice(writedata, strlen(writedata));
    io_ring.IouRingWrite(writeslice, fd, BLOCK_SIZE);

    char writedata2[512] = "gafsafasfadsfgfs";
    writeslice = Slice(writedata2, strlen(writedata));
    io_ring.IouRingWrite(writeslice, fd, BLOCK_SIZE);

    char writedata3[512] = "agkagaefnsjkfkandknf";
    writeslice = Slice(writedata3, strlen(writedata));
    io_ring.IouRingWrite(writeslice, fd, BLOCK_SIZE);
}
