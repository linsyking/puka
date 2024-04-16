#pragma once
#include "sol/sol.hpp"
#include "utils/component.hpp"
namespace Engine {

class LuaComponent : public Component {
public:
    size_t                    lua_vm_id = 0;
    std::optional<sol::table> ref_tbl;
    LuaComponent();
    ~LuaComponent();
    bool is_enabled() override;
    void set_enabled(bool enabled) override;

    void on_collision_enter(Collision &) override;
    void on_collision_exit(Collision &) override;

    void on_trigger_enter(Collision &) override;
    void on_trigger_exit(Collision &) override;

    void set_actor(Actor *actor) override;

    void init() override;
    void on_start() override;
    void update() override;
    void late_update() override;
    void on_destroy() override;
};

using lua_component_ref = std::shared_ptr<LuaComponent>;

}  // namespace Engine
