#include <atomic>
#include <cstdlib>
#include "logger.h"

/*
    not thread safe pool allocator
*/
class memory_pool {
public:
    struct block {
        block* next;
    };

    memory_pool(size_t block_size, size_t num_blocks)
    {
        pool_start = (void*)malloc(num_blocks * block_size);

        if (pool_start == nullptr)
        {
            LOG(LogLevel::ERROR, "malloc failed during memory pool init")
            exit(1);
        }

        free_list = (block*)pool_start;
        block* curr = free_list;
        for (size_t idx = 1; idx < num_blocks; idx++) 
        {
            curr->next = (block*)((char*)pool_start + idx * block_size);
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
        void* res = (block*)free_list;
        free_list = free_list->next;
        return res;
    }

    void deallocate(void* ptr)
    {
        block* curr = free_list;
        free_list = (block*)ptr;
        free_list->next = curr;
    }

    ~memory_pool()
    {
        free(pool_start);
    }

private:
    void* pool_start;
    block* free_list;
};


/*
    thread safe pool allocator
*/
class safe_memory_pool {
public:
    struct block {
        block* next;
    };

    safe_memory_pool(size_t block_size, size_t num_blocks)
    {
        pool_start = malloc(num_blocks * block_size);
        if (pool_start == nullptr)
        {
            LOG(LogLevel::ERROR, "malloc failed during memory pool init")
            exit(1);
        }

        free_list = (block*)pool_start;
        block* curr = free_list;
        for (size_t idx = 1; idx < num_blocks; ++idx) 
        {
            curr->next = (block*)((char*)pool_start + idx * block_size);
            curr = curr->next;
        }
        curr->next = nullptr;
    }

    void* allocate()
    {
        block* old_head = free_list.load(std::memory_order_acquire);

        while (old_head) 
        {
            block* new_head = old_head->next;
            if (free_list.compare_exchange_weak(old_head, new_head, std::memory_order_acquire, std::memory_order_relaxed)) 
            {
                return old_head;
            }
        }

        return nullptr; // Pool is empty
    }

    void deallocate(void* ptr)
    {
        block* new_head = (block*)ptr;
        block* old_head = free_list.load(std::memory_order_acquire);

        do 
        {
            new_head->next = old_head;
        } 
        while (!free_list.compare_exchange_weak(old_head, new_head, std::memory_order_release, std::memory_order_relaxed));
    }

    ~safe_memory_pool()
    {
        free(pool_start);
    }

private:
    void* pool_start;
    std::atomic<block*> free_list;
};
