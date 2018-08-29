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
  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  sumAtomic++;

  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  
}

void WorkloadFullLock()
{
  lock_guard<mutex> lock(mtx);

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  sum++;

  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  
}

void WorkloadScopedLock()
{
  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  {
    lock_guard<mutex> lock(mtx);
    sum++;
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  
}

void Thread(std::function<void(void)> workload)
{
  for (int i=0; i<50; i++)
  {
    workload();
  }
}

int64_t TestWorkload(std::function<void(void)> workload)
{
  std::vector<std::thread> threads;

  high_resolution_clock::time_point t1 = high_resolution_clock::now();

  for (int i=0; i<50; i++)
  {
    threads.push_back(std::thread(Thread, workload));
  }

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