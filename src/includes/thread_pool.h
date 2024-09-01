#include <thread>
#include <vector>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <memory>

class thread_pool
{
public:
    thread_pool(): stop(false)
    {
        for (size_t i = 0; i < std::thread::hardware_concurrency(); i++)
        {
            workers.emplace_back(
                    [this](){
                        for (;;)
                        {
                            std::function<void()> task;
                            {
                                std::unique_lock<std::mutex> ul(m);
                                this->cv.wait(ul, [this](){
                                    return !this->tasks.empty() || this->stop;
                                });
                                if(this->stop)
                                    return;
                                task = std::move(this->tasks.front());
                                this->tasks.pop();
                            } 
                            task();
                        }
                    }
                );
        }
    }
    
    template<class Func, class... Args>
    decltype(auto) enqueue(Func&& f, Args&&... args)
    {
        using return_type = decltype(f(args...));
        auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
        std::future<return_type> future = task->get_future();
        
        {
            std::unique_lock<std::mutex> ul(m);
            if (stop)
            {
                throw std::runtime_error("thread pool destructed");
            }
            tasks.emplace([task](){ (*task)(); });
            cv.notify_one();
        }
        
        return future;
    }
    
    ~thread_pool()
    {
        {
            std::unique_lock<std::mutex> ul(m);
            stop = true;
        }
        cv.notify_all();
        for (auto& worker : workers)
        {
            worker.join();
        }
    }
    
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex m;
    std::condition_variable cv;
    bool stop;
};