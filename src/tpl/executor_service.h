//
// Created by admin on 2021/3/9.
//

#ifndef TPL_SRC_TPL_EXECUTOR_SERVICE_H
#define TPL_SRC_TPL_EXECUTOR_SERVICE_H

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <deque>
#include <future>
#include <thread>
#include <vector>

namespace tpl {

using num_t = unsigned;

class executor_service {
public:
  static struct async_tag_t {} async;
private:

  std::vector<std::thread> threads;

  std::atomic<bool> executing = ATOMIC_VAR_INIT(false);

  std::condition_variable condition;

  std::deque<std::function<void()>> tasks;

  std::mutex mutex;

  static constexpr num_t max_pending_tasks = 512u;

  inline void executing_thread() {
	while (executing) {
	  std::unique_lock<std::mutex> lock(mutex);
	  if (tasks.empty()) {
		condition.wait(lock, [this] { return !tasks.empty() || !executing; });
		if (!executing) return;
	  }
	  std::function<void()> func;
	  func.swap(tasks.front());
	  tasks.pop_front();
	  condition.notify_one();
	  lock.unlock();
	  func.operator()();
	}
  }

  inline void initialize(num_t num_threads) {
	threads.resize(num_threads);
	executing = true;
	for (int i = 0; i < num_threads; ++i) {
	  std::thread([this]() { executing_thread(); }).swap(threads[i]);
	}
  }

  inline void join_all() {
	std::for_each(threads.begin(), threads.end(), [](std::thread &thread) { if (thread.joinable()) thread.join(); });
  }

public:
  /**
   * \brief constructor of thread pool
   * \param num_threads initial number of threads to create
   */
  inline explicit executor_service(num_t num_threads = 0) {
	if (num_threads == 0) num_threads = n_threads_hardware();
	initialize(num_threads);
  }

  /**
   * \brief destructor
   */
  inline ~executor_service() {
	join_all();
  }

  executor_service(const executor_service &) = delete;

  executor_service(executor_service &&) = delete;

  executor_service &operator=(const executor_service &) = delete;

  executor_service &operator=(executor_service &&) = delete;

  /**
   * \brief returns number of threads
   * \return
   */
  [[nodiscard]] inline num_t n_threads() const { return threads.size(); }

  /**
   * \brief returns number of maximum hardware threads
   * \return
   */
  [[nodiscard]] static inline num_t n_threads_hardware() { return std::thread::hardware_concurrency(); }

  /**
   * \brief submit a routine for processing in a thread
   * \tparam Func type function to be invoked
   * \tparam Args types of arguments to pass to the function
   * \param func function to be invoked
   * \param args arguments to pass to the function
   * \return std::future<decltype(func(std::forward<Args>(args)...))>, can be used to retrieve function result
   * \details will block upon full queue
   */
  template<typename Func, typename... Args>
  inline auto submit(Func &&func, Args&& ... args) -> std::future<decltype(func(std::forward<Args>(args)...))> {
	if (!executing) throw;
	std::unique_lock<std::mutex> lock(mutex);
	if (tasks.size() >= max_pending_tasks) {
	  condition.wait(lock, [this] { return tasks.size() < max_pending_tasks || !executing; });
	  if (!executing) throw;
	}
	auto promise = std::make_shared<std::promise<decltype(func(std::forward<Args>(args)...))>>();
	auto future = promise->get_future();
	tasks.template emplace_back([&, func = std::forward<Func>(func), ...args = std::forward<Args>(args),
									promise = std::move(promise)]() mutable {
	  if constexpr (!std::is_void_v<std::invoke_result_t<Func, Args...>>)
		promise->set_value(func(std::forward<Args>(args)...));
	  else {
		func(std::forward<Args>(args)...);
		promise->set_value();
	  }
	});
	condition.notify_one();
	lock.unlock();
	return future;
  }

  /**
   * \brief stop all executing threads
   */
  inline void stop() {
	executing = false;
	std::lock_guard<std::mutex> lock(mutex);
	condition.notify_all();
  }

  /**
   * \brief stop all executing threads after a specified time period
   * \tparam Rep
   * \tparam Period
   * \param duration
   */
  template<typename Rep, typename Period>
  inline void schedule_for(std::chrono::duration<Rep, Period> duration) {
	std::this_thread::sleep_for(duration);
	stop();
  }

  /**
   * \brief stop all executing threads at a specified time point
   * \tparam Clock
   * \tparam Duration
   * \param tp
   */
  template<typename Clock, typename Duration>
  inline void schedule_until(std::chrono::time_point<Clock, Duration> tp) {
	std::this_thread::sleep_until(tp);
	stop();
  }

  /**
   * \brief stop all executing threads and
   * \param num_threads
   */
  inline void resize(num_t num_threads) {
	if (num_threads > threads.size()) {
	  auto origin = threads.size();
	  threads.resize(num_threads);
	  for (int i = origin; i < num_threads; ++i) {
		std::thread([this]() { executing_thread(); }).swap(threads[i]);
	  }
	} else {
	  stop();
	  join_all();
	  initialize(num_threads);
	}
  }

  /**
   * \brief resume executing
   */
  inline void resume() {
	if (executing) return;
	join_all();
	initialize(n_threads());
  }
}; // class executor_service
} // namespace tpl

#endif //TPL_SRC_TPL_EXECUTOR_SERVICE_H
