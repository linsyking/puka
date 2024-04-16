#pragma once

#include <memory>
#include <string>
#include "mgrs/box2d_mgr.hpp"
#include "task_runner/task.hpp"

namespace Engine {
class Component {
public:
    Component() {}
    virtual ~Component(){};

    virtual bool is_enabled()              = 0;
    virtual void set_enabled(bool enabled) = 0;
    void         die();

    /// Component type
    std::string type;

    /// Component key (name)
    std::string key;

    /// Actor name
    std::string actor_name;

    bool has_start       = false;
    bool has_update      = false;
    bool has_late_update = false;
    bool has_destroy     = false;

    /// Update component metainfo
    virtual void init() {}
    virtual void on_start() {}
    virtual void update() {}
    virtual void late_update() {}
    virtual void on_destroy() {}

    bool has_collision_enter = false;
    bool has_collision_exit  = false;

    bool has_trigger_enter = false;
    bool has_trigger_exit  = false;

    virtual void on_collision_enter(Collision &) {}
    virtual void on_collision_exit(Collision &) {}

    virtual void on_trigger_enter(Collision &) {}
    virtual void on_trigger_exit(Collision &) {}

    virtual void set_actor(Actor *actor) = 0;

    task_ptr onstart_task;
    task_ptr update_task;
    task_ptr late_update_task;

protected:
    bool dead = false;
};

using component_ref = std::shared_ptr<Component>;
}  // namespace Engine
