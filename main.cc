#include <iostream>
#include <string>
#include <chrono>
#include "thread_pool.hpp"

void safe_print(const std::string& str) {
    static std::mutex cout_mutex;
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cout << str << '\n';
}

int main() {
    thread_pool pool(std::thread::hardware_concurrency());

    pool.schedule([]() { std::this_thread::sleep_for(std::chrono::seconds(10)); safe_print("hello world"); });
    pool.schedule([]() { std::this_thread::sleep_for(std::chrono::seconds(10)); safe_print("hello world"); });
    pool.schedule([]() { std::this_thread::sleep_for(std::chrono::seconds(10)); safe_print("hello world"); });
    pool.schedule([]() { std::this_thread::sleep_for(std::chrono::seconds(10)); safe_print("hello world"); });
}