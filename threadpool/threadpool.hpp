/****************************************************************************
The MIT license
Copyright © 2017 Maks Mazurov (fox.cpp) 
Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
****************************************************************************/

#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <functional>
#include <queue>
#include <mutex>                  
#include <thread>
#include <conditional_variable>

/// The 'thread_pool' class manages queue of jobs (actually functions) to be executed later in one
/// of worker threads. Exact execution time and thread is undefined and actually random.
/// But jobs executed in queue (FIFO) maner.
class thread_pool {
public:
    /// Creates threads_count workers.
    thread_pool(unsigned threads_count);

    /// Stops all workers. Locks if there are running jobs.
    ~thread_pool();

    /// Schedules execution of function, exact execution time and thread is undefined.
    /// Locks if queue accessed from other place.
    void schedule(std::function<void()> func);

private:
    static void thread_entry(thread_pool& controller);

    logger balancer_logger;

    /// Set to true to ask workers to stop.
    bool stop = false;

    std::vector<std::thread> threads;
    std::queue<std::function<void()> > queue;

    std::mutex queue_access_lock;
    std::condition_variable workers_cv;
};

#endif // THREAD_POOL_HPP