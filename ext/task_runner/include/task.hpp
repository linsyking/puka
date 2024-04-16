#pragma once

#include <memory>
#include <optional>
#include <shared_mutex>
#include <vector>

class task;
using task_ptr = std::shared_ptr<task>;

class task {
public:
    /// If not null, run this task on a specific thread
    std::optional<size_t> run_on = std::nullopt;
    virtual void          run()  = 0;
    virtual ~task(){};
};

struct task_ex;
using task_ex_ptr = std::shared_ptr<task_ex>;

struct task_ex {
    /// Successors
    std::vector<task_ex_ptr> succ;

    /// Runtime predessor number
    size_t pred_num = 0;

    /// Reader-writer lock for pred_num
    std::shared_mutex mtx;

    task_ptr self;
};
