#pragma once
#include <mutex>
#include "mgrs/actor.hpp"
#include "utils/component.hpp"
namespace Engine {

class BuiltinComponent : public Component {
public:
    BuiltinComponent();
    virtual ~BuiltinComponent(){};
    Actor *actor   = nullptr;
    bool   enabled = true;

    /// Mutex to access the built-in component
    std::mutex   mtx;
    virtual bool is_enabled() override;
    virtual void set_enabled(bool enabled) override;

    virtual void on_collision_enter(Collision &) override {}
    virtual void on_collision_exit(Collision &) override {}

    virtual void on_trigger_enter(Collision &) override {}
    virtual void on_trigger_exit(Collision &) override {}

    virtual void init() override {}
    virtual void on_start() override {}
    virtual void update() override {}
    virtual void late_update() override {}
    virtual void on_destroy() override {}

    virtual void set_actor(Actor *actor) override { this->actor = actor; };
};

}  // namespace Engine
