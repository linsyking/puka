#pragma once

#include <memory>
#include <mutex>
#include <shared_mutex>
namespace Engine {

class unique_mutex {
public:
    std::mutex &get() { return *mtx; }
    unique_mutex(const unique_mutex &) { mtx = std::make_unique<std::mutex>(); }
    unique_mutex &operator=(const unique_mutex &) {
        mtx = std::make_unique<std::mutex>();
        return *this;
    }
    // Delete move constructor and move assignment
    unique_mutex(unique_mutex &&)            = delete;
    unique_mutex &operator=(unique_mutex &&) = delete;

    unique_mutex() {}
    std::unique_ptr<std::mutex> mtx = std::make_unique<std::mutex>();
};

class shared_mutex {
public:
    std::shared_mutex &get() { return *mtx; }
    shared_mutex(const shared_mutex &) { mtx = std::make_unique<std::shared_mutex>(); }
    shared_mutex &operator=(const shared_mutex &) {
        mtx = std::make_unique<std::shared_mutex>();
        return *this;
    }
    // Delete move constructor and move assignment
    shared_mutex(shared_mutex &&)            = delete;
    shared_mutex &operator=(shared_mutex &&) = delete;

    shared_mutex() {}
    std::unique_ptr<std::shared_mutex> mtx = std::make_unique<std::shared_mutex>();
};

}  // namespace Engine
