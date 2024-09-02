#include "logger.h"

class memory_pool {
public:
    struct Block {
        Block* next;
    };

    memory_pool(size_t block_size, size_t num_blocks)
    {
        pool_start = (void*)malloc(num_blocks * block_size);

        if (pool_start == nullptr)
        {
            LOG(LogLevel::ERROR, "malloc failed during memory pool init")
            exit(1);
        }

        free_list = (Block*)pool_start;
        Block* curr = free_list;
        for (size_t idx = 1; idx < num_blocks; idx++) 
        {
            curr->next = (Block*)((char*)pool_start + idx * block_size);
            curr = curr->next;
        }
        curr->next = nullptr;
    }

    void* allocate()
    {
        if (!free_list)
        {
            return nullptr;
        }

        void* res = (Block*)free_list;
        free_list = free_list->next;
        return res;
    }

    void deallocate(void* block)
    {
        Block* curr = free_list;
        free_list = (Block*)block;
        free_list->next = curr;
    }

    ~memory_pool()
    {
        free(pool_start);
    }

private:
    size_t block_size_;
    size_t num_blocks_;
    void* pool_start;
    Block* free_list;
};