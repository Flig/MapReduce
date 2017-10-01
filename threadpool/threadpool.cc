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

#include "thread_pool.hpp"

thread_pool::thread_pool(unsigned threads_count) {
    while (threads_count --> 0) {
        threads.emplace_back(thread_pool::thread_entry, std::ref(*this));
    }
}

thread_pool::~thread_pool() {
    stop = true;
    workers_cv.notify_all();
    for (std::thread& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void thread_pool::schedule(std::function<void()> func) {
    std::lock_guard<std::mutex> lock(queue_access_lock);
    queue.push(func);
    workers_cv.notify_one();
}

void thread_pool::thread_entry(thread_pool& controller) {
    std::function<void()> job;
    while (true) { // Actual loop exit happens at line 40 (if controller set stop flag).

        { // Wait for job to appear in queue.
            std::unique_lock<std::mutex> lock(controller.queue_access_lock);

            while (!controller.stop && controller.queue.empty()) {
                controller.workers_cv.wait(lock);
            }
            if (controller.stop) {
                return;
            }

            job = controller.queue.front();
            controller.queue.pop();
        }

        job();
    }
}