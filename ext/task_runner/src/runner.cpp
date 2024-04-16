#include "runner.hpp"
#include <algorithm>
#include <mutex>
#include <shared_mutex>
#include <stdexcept>
#include "task.hpp"

size_t runner::thread_num() {
    runner         &r  = get();
    std::thread::id id = std::this_thread::get_id();
    for (size_t i = 0; i < r.threads.size(); ++i) {
        if (r.threads[i].get_id() == id) {
            return i;
        }
    }
    throw std::runtime_error("Thread not found");
}

void runner::task_single_runner() {
    runner &r         = get();
    size_t  thread_id = r.thread_num();

    std::unique_lock<std::mutex> lock(r.runtime_mtx);
    while (r.started) {
        task_ex_ptr task;
        while (1) {
            if (r.runtime.has_value() && (!r.runtime->all_named_tasks[thread_id].empty() ||
                                          !r.runtime->all_unnamed_tasks.empty())) {
                // Get a task
                auto &named_tasks   = r.runtime->all_named_tasks[thread_id];
                auto &unnamed_tasks = r.runtime->all_unnamed_tasks;
                for (auto it = named_tasks.begin(); it != named_tasks.end(); ++it) {
                    std::shared_lock<std::shared_mutex> _((*it)->mtx);
                    if ((*it)->pred_num == 0) {
                        task = *it;
                        named_tasks.erase(it);
                        break;
                    }
                }
                if (!task) {
                    // Find unnamed task
                    for (auto it = unnamed_tasks.begin(); it != unnamed_tasks.end(); ++it) {
                        std::shared_lock<std::shared_mutex> _((*it)->mtx);
                        if ((*it)->pred_num == 0) {
                            task = *it;
                            unnamed_tasks.erase(it);
                            break;
                        }
                    }
                }
                if (task) {
                    // Found a task, break
                    break;
                }
            }
            r.has_task.wait(lock);
            if (!r.started) {
                return;
            }
        }
        lock.unlock();
        task->self->run();
        for (auto &t : task->succ) {
            // Require a writer lock
            std::unique_lock<std::shared_mutex> _(t->mtx);
            t->pred_num--;
            if (t->pred_num == 0) {
                r.has_task.notify_all();
            }
        }
        lock.lock();
        r.runtime->task_remaining--;
        if (r.runtime->task_remaining == 0) {
            if (r.to_run.empty()) {
                r.runtime = std::nullopt;
                r.all_done.notify_all();
            } else {
                r.runtime = r.to_run.front();
                r.to_run.pop();
                r.has_task.notify_all();
            }
        }
    }
}

void runner::boot(size_t num_threads) {
    runner &r = runner::get();
    if (r.started) {
        return;
    }
    r.started = true;
    r.threads.reserve(num_threads);
    for (size_t i = 0; i < num_threads; ++i) {
        r.threads.emplace_back(task_single_runner);
    }
}

void runner::add_task(task_ptr a) {
    runner &r = get();
    if (!r.started) {
        return;
    }
    if (r.task_map.find(a) == r.task_map.end()) {
        if (a->run_on.has_value() && a->run_on.value() >= r.threads.size()) {
            throw std::runtime_error("Thread index out of range");
        }
        r.task_map[a]       = std::make_shared<task_ex>();
        r.task_map[a]->self = a;
        r.all_tasks.insert(r.task_map[a]);
    }
}

void runner::add_order(task_ptr a, task_ptr b) {
    runner &r = get();
    r.add_task(a);
    r.add_task(b);
    if (!r.started) {
        return;
    }
    for (auto &t : r.task_map[a]->succ) {
        if (t == r.task_map[b]) {
            return;
        }
    }
    r.task_map[b]->pred_num++;
    r.task_map[a]->succ.push_back(r.task_map[b]);
}

void runner::commit() {
    runner &r = get();
    if (!r.started || r.all_tasks.empty()) {
        return;
    }
    task_runtime rt;
    rt.all_named_tasks.resize(r.threads.size());
    for (auto &task : r.all_tasks) {
        if (task->self->run_on.has_value()) {
            rt.all_named_tasks[task->self->run_on.value()].push_back(task);
        } else {
            rt.all_unnamed_tasks.push_back(task);
        }
    }
    // Sort for better performance
    std::sort(rt.all_unnamed_tasks.begin(), rt.all_unnamed_tasks.end(),
              [](task_ex_ptr &a, task_ex_ptr &b) { return a->pred_num < b->pred_num; });
    for (size_t i = 0; i < rt.all_named_tasks.size(); ++i) {
        std::sort(rt.all_named_tasks[i].begin(), rt.all_named_tasks[i].end(),
                  [](task_ex_ptr &a, task_ex_ptr &b) { return a->pred_num < b->pred_num; });
    }
    rt.task_remaining = r.all_tasks.size();
    {
        std::unique_lock<std::mutex> lock(r.runtime_mtx);
        if (r.runtime.has_value()) {
            r.to_run.push(rt);
        } else {
            r.runtime = rt;
        }
    }
    r.task_map.clear();
    r.all_tasks.clear();
    r.running = true;
    r.has_task.notify_all();
}

bool runner::is_all_done() {
    // Requires lock acquired beforehand
    runner &r = get();
    return !r.runtime.has_value() && r.to_run.empty();
}

void runner::wait() {
    runner &r = get();
    if (!r.started || !r.running) {
        return;
    }
    std::unique_lock<std::mutex> lock(r.runtime_mtx);
    while (!r.is_all_done()) {
        r.all_done.wait(lock);
    }
    // No more tasks, cleanup
    r.task_map.clear();
    r.all_tasks.clear();
    r.runtime = std::nullopt;
    r.running = false;
}

void runner::quit() {
    runner &r = get();
    if (!r.started) {
        return;
    }
    if (r.running) {
        wait();
    }
    {
        std::unique_lock<std::mutex> lock(r.runtime_mtx);
        r.started = false;
    }
    r.has_task.notify_all();
    for (auto &t : r.threads) {
        t.join();
    }
    r.threads.clear();
}
