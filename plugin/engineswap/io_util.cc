#include "io_util.h"
#include <iostream>

namespace rocksdb{

int IouRing::IouRingRead(size_t n, Slice* result, char* scratch, int block_size, int fd, off_t read_offset)
    {
        off_t offset = 0;
        // submit requests per block
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
            iovecvec[current_block].iov_base = (void*)(scratch + offset);
            
            offset += bytes_to_read;
            current_block++;
            bytes_remaining -= bytes_to_read;
        }
        
        struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
        io_uring_prep_readv(sqe, fd, iovecvec, blocks, read_offset);
        io_uring_sqe_set_data(sqe, iovecvec);
        io_uring_submit(&this->ring);

        struct io_uring_cqe *cqe;
        int ret = io_uring_wait_cqe(&ring, &cqe);
        if (ret < 0) {
            std::cout << ("io_uring_wait_cqe\n");
            io_uring_cqe_seen(&ring, cqe);
            free(iovecvec);
            return -1;
        }
        if (cqe->res < 0) {
            std::cout << "Async readv fail result: " << cqe->res << " target " << n << "\n";
            io_uring_cqe_seen(&ring, cqe);
            free(iovecvec);
            return -1;
        }

        iovecvec = (struct iovec*)io_uring_cqe_get_data(cqe);
        
        *result = Slice(scratch, cqe->res);
        free(iovecvec);
        io_uring_cqe_seen(&ring, cqe); //stuff in the cqe cannot be accessed anymore, 
        return 0;
    } 

    int IouRing::IouRingWrite(const Slice& data, int fd, int block_size)
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
            iovecvec[current_block].iov_base = (void*)(data.data() + offset);

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
        if (cqe->res > 0 && cqe->res != (signed long)data.size()) { // is there any situation where this should not be right?
            std::cout <<  "Async writev failed " << cqe->res << "\n";
            std::cout << "write result: " << cqe->res << " target " << data.size() << "\n";
            io_uring_cqe_seen(&ring, cqe);
            free(iovecvec);
            return -1;
        }

        io_uring_cqe_seen(&ring, cqe);
        free(iovecvec);
        return 0;
    }

    #pragma region //non vectorised

    int IouRing::IouNvRingWrite(const Slice& data, int fd)
    {
        
        struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
        io_uring_prep_write(sqe, fd, data.data(), data.size(), 0);
        io_uring_sqe_set_data(sqe, (void*)data.data());
        io_uring_submit(&this->ring);

        struct io_uring_cqe *cqe;
        int ret = io_uring_wait_cqe(&ring, &cqe);
        if (ret < 0) {
            std::cout <<"io_uring_wait_cqe \n";
            io_uring_cqe_seen(&ring, cqe);
            
            return -1;
        }
        if (cqe->res > 0 && cqe->res != (signed long)data.size()) { // is there any situation where this should not be right?
            std::cout <<  "Async write failed " << cqe->res << "\n";
            std::cout << "write result: " << cqe->res << " target " << data.size() << "\n";
            io_uring_cqe_seen(&ring, cqe);
            
            return -1;
        }

        io_uring_cqe_seen(&ring, cqe);
        
        return 0;
    }

    int IouRing::IouNvRingRead(size_t n, Slice* result, char* scratch, int fd, off_t read_offset)
    {

        struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
        io_uring_prep_read(sqe, fd, scratch, n, read_offset);
        io_uring_sqe_set_data(sqe, (void *)scratch);
        io_uring_submit(&this->ring);

        struct io_uring_cqe *cqe;
        int ret = io_uring_wait_cqe(&ring, &cqe);
        if (ret < 0) {
            std::cout << ("io_uring_wait_cqe\n");
            io_uring_cqe_seen(&ring, cqe);
            
            return -1;
        }
        if (cqe->res < 0) {
            std::cout << "Async read fail result: " << cqe->res << " target " << n << "\n";
            io_uring_cqe_seen(&ring, cqe);
            
            return -1;
        }

        scratch = (char*)io_uring_cqe_get_data(cqe);
        
        *result = Slice(scratch, cqe->res);
        
        io_uring_cqe_seen(&ring, cqe); //stuff in the cqe cannot be accessed anymore, 
        return 0;
    } 

    #pragma endregion
}