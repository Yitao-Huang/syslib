#pragma once
#include <memory>
#include <mutex>

template <typename T>
class on_demand
{
public:
    on_demand() = default;
    ~on_demand() = default;
    on_demand(const on_demand& rhs) = delete;
    on_demand(on_demand&& rhs) = delete;
    on_demand& operator=(const on_demand& rhs) = delete;
    on_demand& operator=(on_demand&& rhs) = delete;

    T* read() const
    {
        return data.get();
    }

    T* write()
    {
        if (!data.get())
        {
            data.reset(new T());
        }
        return data.get();
    }

    T* safe_read() const
    {
        std::unique_lock<std::mutex> ul(this->m);
        return read();
    }

    T* safe_write()
    {
        std::unique_lock<std::mutex> ul(this->m);
        return write();
    }

private:
    std::unique_ptr<T> data;
    std::mutex m;
};