#pragma once
#include <atomic>
#include <vector>

template<typename T>
class spsc_queue {
public:
    spsc_queue(size_t size): size_(size), head_(0), tail_(0), buffer_(size) {}

    bool enqueue(T&& data) {
        int curr_head = head_.load(std::memory_order_relaxed);
        int next_head = next(curr_head);

        if (next_head == tail_.load(std::memory_order_acquire)) {
            return false;
        }

        buffer_[curr_head] = std::move(data);
        head_.store(next_head, std::memory_order_release);
        return true;
    }

    bool dequeue(T& data) {
        int curr_tail = tail_.load(std::memory_order_relaxed);
        if (curr_tail == head_.load(std::memory_order_acquire)) {
            return false;
        }

        data = buffer_[curr_tail];
        tail_.store(next(curr_tail), std::memory_order_release);
        return true;
    }

private:
    int next(int curr) {
        return (curr + 1) % size_;
    }

    size_t size_;
    std::atomic<int> head_;
    std::atomic<int> tail_;
    std::vector<T> buffer_;
};


template<typename T>
class mpsc_queue {
public:
    mpsc_queue(size_t size) 
        : buffer_(size), size_(size), head_(0), tail_(0) {
            if (size == 0) {
                throw std::invalid_argument("Capacity must be greater than zero");
            }
        }

    bool enqueue(T&& value) {
        size_t current_head = head_.load(std::memory_order_relaxed);
        size_t next_head = (current_head + 1) % size_;
        // Reserve a slot for this producer (atomic increment of head)
        do {
            if (next_head == tail_.load(std::memory_order_acquire)) {
                return false; // Queue is full
            }
            next_head = (current_head + 1) % size_;
        } while (!head_.compare_exchange_weak(current_head, next_head, std::memory_order_acquire, std::memory_order_relaxed));

        // Write the value into the reserved slot
        buffer_[current_head] = std::move(value);

        // Operation successful
        return true;
    }

    bool dequeue(T& result) {
        size_t current_tail = tail_.load(std::memory_order_relaxed);

        // If the queue is empty
        if (current_tail == head_.load(std::memory_order_acquire)) {
            return false; // Queue is empty
        }

        // Retrieve the value from the current tail
        result = buffer_[current_tail];

        // Atomically update the tail (release)
        tail_.store((current_tail + 1) % size_, std::memory_order_release);

        return true;
    }

private:
    std::vector<T> buffer_;
    const size_t size_;
    std::atomic<size_t> head_;
    std::atomic<size_t> tail_;
};
