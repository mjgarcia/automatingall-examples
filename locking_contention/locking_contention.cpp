#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <functional>

using namespace std;
using namespace std::chrono;

mutex mtx;
int sum = 0;
std::atomic<int> sumAtomic(0);

void WorkloadAtomic()
{
  // This simulates work done before working on the shared resource
  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  sumAtomic++;

  // This simulates work done after working on the shared resource
  std::this_thread::sleep_for(std::chrono::milliseconds(1));

}

void WorkloadFullLock()
{
  lock_guard<mutex> lock(mtx);

  // This simulates work done before working on the shared resource
  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  sum++;

  // This simulates work done after working on the shared resource
  std::this_thread::sleep_for(std::chrono::milliseconds(1));

}

void WorkloadScopedLock()
{
  // This simulates work done before working on the shared resource
  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  {
    lock_guard<mutex> lock(mtx);
    sum++;
  }

  // This simulates work done after working on the shared resource
  std::this_thread::sleep_for(std::chrono::milliseconds(1));

}

void Thread(std::function<void(void)> workload)
{
  // Simulate thread working on shared resource repeatedly

  for (int i=0; i<50; i++)
  {
    workload();
  }
}

int64_t TestWorkload(std::function<void(void)> workload)
{
  std::vector<std::thread> threads;

  high_resolution_clock::time_point t1 = high_resolution_clock::now();

  // Create all worker threads

  for (int i=0; i<50; i++)
  {
    threads.push_back(std::thread(Thread, workload));
  }

  // Wait for all thread to finish

  for (auto& t : threads)
  {
    t.join();
  }

  high_resolution_clock::time_point t2 = high_resolution_clock::now();

  auto duration = duration_cast<chrono::milliseconds>( t2 - t1 ).count();

  return duration;
}

void RunExperiment(std::function<void(void)> workload)
{
  int count = 10;
  int64_t sum = 0;

  // Run experiments several times and take average

  for (int i=0; i<count; i++)
  {
    sum += TestWorkload(workload);
  }

  cout << "Average duration in ms " << sum/count << endl;
}

int main()
{
  RunExperiment(WorkloadFullLock);

  RunExperiment(WorkloadScopedLock);

  RunExperiment(WorkloadAtomic);
}