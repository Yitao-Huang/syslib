#include <atomic>
#include <vector>

template<typename T>
class spsc_queue {
public:
    spsc_queue(size_t size): size_(size), head_(0), tail_(0), buffer_(size) {}

    bool enqueue(const T& data) {
        int curr_head = head_.load(std::memory_order_relaxed);
        int next_head = next(curr_head);

        if (next_head == tail_.load(std::memory_order_acquire)) {
            return false;
        }

        buffer_[curr_head] = data;
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
private:
    struct node {
        T data;
        std::atomic<node*> next;

        node() : next(nullptr) {}
        node(const T& value) : data(value), next(nullptr) {}
        node(T&& value) : data(std::move(value)), next(nullptr) {}
    };

    std::atomic<node*> head;
    node* tail;

public:
    mpsc_queue() : head(new node()), tail(head.load()) {}

    ~mpsc_queue() {
        while (node* old_head = head.load()) {
            head.store(old_head->next);
            delete old_head;
        }
    }

    void enqueue(const T& value) {
        node* new_node = new node(value);
        node* old_head = head.exchange(new_node);
        old_head->next.store(new_node, std::memory_order_release);
    }

    void enqueue(T&& value) {
        node* new_node = new node(std::move(value));
        node* old_head = head.exchange(new_node);
        old_head->next.store(new_node, std::memory_order_release);
    }

    bool dequeue(T& result) {
        node* old_tail = tail;
        node* next = old_tail->next.load(std::memory_order_acquire);
        if (next) {
            result = std::move(next->data);
            tail = next;
            delete old_tail;
            return true;
        }
        return false;
    }
};


template <typename T>
class mpmc_queue {
public:
    explicit mpmc_queue(size_t size)
        : size_(size), buffer_(size), head_(0), tail_(0) {
        for (size_t i = 0; i < size_; ++i) {
            buffer_[i].sequence.store(i, std::memory_order_relaxed);
        }
    }

    bool enqueue(const T& item)
    {
        size_t head = head_.load(std::memory_order_relaxed);
        for (;;) {
            Cell& cell = buffer_[head % size_];
            size_t seq = cell.sequence.load(std::memory_order_acquire);
            intptr_t diff = (intptr_t)seq - (intptr_t)head;
            if (diff == 0) 
            {
                if (head_.compare_exchange_weak(head, head + 1, std::memory_order_relaxed)) {
                    cell.value = item;
                    cell.sequence.store(head + 1, std::memory_order_release);
                    return true;
                }
            } 
            else if (diff < 0) 
            {
                return false; /* full */
            } 
            else {
                head = head_.load(std::memory_order_relaxed);
            }
        }
    }

    bool dequeue(T& data) 
    {
        size_t tail = tail_.load(std::memory_order_relaxed);
        for (;;) 
        {
            Cell& cell = buffer_[tail % size_];
            size_t seq = cell.sequence.load(std::memory_order_acquire);
            intptr_t diff = (intptr_t)seq - (intptr_t)(tail + 1);
            if (diff == 0) 
            {
                if (tail_.compare_exchange_weak(tail, tail + 1, std::memory_order_relaxed)) 
                {
                    data = cell.value;
                    cell.sequence.store(tail + size_, std::memory_order_release);
                    return true;
                }
            } 
            else if (diff < 0) 
            {
                return false; /* empty */
            } 
            else 
            {
                tail = tail_.load(std::memory_order_relaxed);
            }
        }
    }

private:
    struct Cell 
    {
        std::atomic<size_t> sequence;
        T value;
    };

    const size_t size_;
    std::vector<Cell> buffer_;
    std::atomic<size_t> head_;
    std::atomic<size_t> tail_;
};