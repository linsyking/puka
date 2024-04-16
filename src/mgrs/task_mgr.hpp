#pragma once

#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <unordered_map>
#include "mgrs/lua_runner.hpp"
#include "task_runner/task.hpp"
#include "utils/builtin_component.hpp"
#include "utils/component.hpp"
#include "utils/lua_component.hpp"
#include "utils/mutex_wrapper.hpp"

namespace Engine {

class TaskManager {
private:
    std::queue<component_ref> on_start_tasks;

public:
    static TaskManager &get();
    static LuaRunner   &get_lua_runner();
    static LuaRunner   &get_lua_runner(size_t id);
    void                init();
    void                quit();

    void new_component(std::shared_ptr<LuaComponent>);

    static void init_state(sol::state &state);

    std::queue<component_ref> &get_on_start_components() { return on_start_tasks; }

    /// Lua runners
    std::vector<LuaRunner> runners;

    /// Mutex to protect shared data
    std::mutex mtx;

    size_t find_next_vm();

    /// Map component type to a VM
    std::unordered_map<std::string, size_t> component_vmmap;
};

class builtin_update_task : public task {
private:
    BuiltinComponent *c;

public:
    builtin_update_task(BuiltinComponent *c) : c(c) {}
    void run() override { c->update(); }
};

class builtin_onstart_task : public task {
private:
    BuiltinComponent *c;

public:
    builtin_onstart_task(BuiltinComponent *c) : c(c) {}
    void run() override { c->on_start(); }
};

class builtin_late_update_task : public task {
private:
    BuiltinComponent *c;

public:
    builtin_late_update_task(BuiltinComponent *c) : c(c) {}
    void run() override { c->late_update(); }
};

class lua_onstart_task : public task {
private:
    LuaComponent *c;

public:
    lua_onstart_task(LuaComponent *c) : c(c) { run_on = c->lua_vm_id; }
    void run() override {
        std::unique_lock<std::mutex> lock(TaskManager::get_lua_runner(c->lua_vm_id).mtx.get());
        c->on_start();
    }
};

class lua_update_task : public task {
private:
    LuaComponent *c;

public:
    lua_update_task(LuaComponent *c) : c(c) { run_on = c->lua_vm_id; }
    void run() override {
        std::unique_lock<std::mutex> lock(TaskManager::get_lua_runner(c->lua_vm_id).mtx.get());
        c->update();
    }
};

class lua_late_update_task : public task {
private:
    LuaComponent *c;

public:
    lua_late_update_task(LuaComponent *c) : c(c) { run_on = c->lua_vm_id; }
    void run() override {
        std::unique_lock<std::mutex> lock(TaskManager::get_lua_runner(c->lua_vm_id).mtx.get());
        c->late_update();
    }
};

class init_vm_task : public task {
public:
    void run() override;
};

}  // namespace Engine
