#ifndef __thread_pool_hpp__
#define __thread_pool_hpp__

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <list>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

class ThreadPool {
public:
  ThreadPool() = delete;
  ThreadPool(const ThreadPool &) = delete;
  ThreadPool(size_t N) : done(false), threads(N) {
    for (auto &th : threads)
      th = std::thread(&ThreadPool::waitForTask, this);
  }

  virtual ~ThreadPool() {
    done.store(true);

    Task emptyTask(new std::function<void()>([]() { return; }));
    for (size_t i = 0; i < threads.size(); i++)
      queue.addTask(emptyTask);

    for (auto &th : threads)
      th.join();
  }

  template <class F, class... Args>
  auto addTask(F &&f, Args &&... args) -> std::future<decltype(f(args...))> {
    auto pkg = std::make_shared<std::packaged_task<decltype(f(args...))()>>(
        std::bind(f, args...));
    auto task = Task(new std::function<void()>([pkg]() { (*pkg)(); }));
    queue.addTask(task);
    return pkg->get_future();
  }

  size_t size() const {
    return threads.size();
  }

private:
  std::atomic_bool done;

  void waitForTask() {
    while (!done.load())
      std::invoke(*(queue.getTask()));
  }

  using Threads = std::vector<std::thread>;
  Threads threads;

  template <class Task> class TaskQueue {
  public:
    void addTask(const Task &task) {
      std::unique_lock<std::mutex> lock(queueMtx);
      queue.push_back(task);
      queueHasTask.notify_all();
    }

    Task getTask() {
      std::unique_lock<std::mutex> lock(queueMtx);
      queueHasTask.wait(lock, [&] { return queue.size(); });

      Task task = queue.front();
      queue.pop_front();
      return task;
    }

  private:
    using Queue = std::list<Task>;
    Queue queue;
    std::mutex queueMtx;
    std::condition_variable queueHasTask;
  };

  using Task = std::shared_ptr<std::function<void()>>;
  TaskQueue<Task> queue;
};

#endif /* ifndef __thread_pool_hpp__ */

