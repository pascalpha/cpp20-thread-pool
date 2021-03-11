#include <iostream>
#include <chrono>
#include <thread>

#include <tpl/executor_service.h>

// demo program

int main() {
  //create a thread pool with a maximum hardware threads
  tpl::executor_service service;

  // resize it to contain 5 threads
  service.resize(5);

  // to hold invocation results
  std::vector<std::future<void>> futures;
  futures.reserve(10);

  // not pollute standard output with interfering outputs
  std::mutex mutex;

  // enqueue 100 long-running tasks
  for (int i = 0; i < 10; ++i) {
	futures.emplace_back(service.submit([&](int x) -> decltype(auto) {
	  // emulate long processing
	  std::this_thread::sleep_for(std::chrono::seconds(1));

	  // print the result
	  std::lock_guard<std::mutex> lock(mutex);
	  std::cout << x << std::endl;
	}, i));
  }

  // get the results
  std::for_each(futures.begin(), futures.end(), [](std::future<void> &future) { future.get(); });

  // stop after 1 second
  service.execute_for(std::chrono::seconds(1));

  // resume working
  service.resume();

  // to hold results
  std::vector<std::future<int>> futures2;
  futures2.reserve(10);

  // enqueue 100 long-running tasks that return
  for (int i = 0; i < 10; ++i) {
	futures2.emplace_back(service.submit([&](int x) -> decltype(auto) {
	  // emulate long processing
	  std::this_thread::sleep_for(std::chrono::seconds(1));
	  return x * 2;
	}, i));
  }

  // print the results
  std::for_each(futures2.begin(), futures2.end(), [](std::future<int> &future) { std::cout << future.get() << std::endl; });

  // stop after 1 second
  service.execute_until(std::chrono::system_clock::now() + std::chrono::seconds(1));
}
